#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <functional>
#include <cstdio>
#include <cstring>
#include <list>
int main(int argc, char** argv)
{
    std::string outputfn = "extrasrc/fpaux.hh";
    std::vector<std::string> inputs;

    for(int a=1; a<argc; ++a)
    {
        if(std::strcmp(argv[a], "-o") == 0)
        {
            if(++a == argc)
            {
                std::cerr << "build_fpaux: Expected output filename after -o\n";
                return 1;
            }
            outputfn = argv[a];
            continue;
        }
        else
        {
            inputs.push_back(argv[a]);
            continue;
        }
    }

    // Parse the input files.
    std::map<std::string/*name*/,
        std::tuple<std::string/*contents*/,
                   std::set<std::string>,/*dependencies*/
                   unsigned/*color*/,
                   unsigned/*line count*/,
                   unsigned/*first lineno*/
                 >> modules;
    for(const auto& fn: inputs)
    {
        std::ifstream ifs(fn);
        std::string module_name = fn;
        std::size_t p = module_name.rfind('/');
        if(p != module_name.npos) module_name.erase(0, p+1);
        p = module_name.find('.');
        if(p != module_name.npos) module_name.erase(p);

        auto& mod = modules[module_name];
        std::string& contents = std::get<0>(mod);
        auto&    dependencies = std::get<1>(mod);
        unsigned& linecount   = std::get<3>(mod);
        unsigned& firstline   = std::get<4>(mod);
        firstline = 1;

        for(std::string line; std::getline(ifs, line); )
        {
            if(line.substr(0, 8) == "//$DEP: ")
            {
                dependencies.insert(line.substr(8));
                ++firstline;
            }
            else if(line.empty() && linecount == 0)
                ++firstline;
            else
            {
                contents += line;
                contents += '\n';
                ++linecount;
            }
        }
    }

    // Create a topological order for the modules
    std::vector<std::string> order;
    {std::list<std::string> cg;
    std::function<void(const std::string&)> dfs = [&](const std::string& from)
    {
        auto i = modules.find(from);
        if(i == modules.end())
        {
            std::cerr << "Module " << from << " not found\n";
            std::abort();
        }
        auto& module = i->second;
        std::set<std::string>& deps = std::get<1>(module);
        unsigned& color             = std::get<2>(module);
        if(color == 2) return; // ok
        cg.push_back(from);
        if(color == 1)
        {
            std::cerr << "Error: Cyclical dependency to " << from << '\n';
            for(auto& m: cg) std::cerr << " -> " << m;
            std::cerr << '\n';
            std::abort(); // ERROR: loop detected
        }
        color = 1;
        for(auto& dep: deps) { dfs(dep); }
        cg.pop_back();
        order.push_back(from);
        color = 2;
    };
    for(auto& m: modules)
        if(std::get<2>(m.second) == 0)
            dfs(m.first);}

    std::string tempfn = outputfn + ".new";
    {std::ifstream ifs(outputfn);
    std::ofstream ofs(tempfn);
    bool inside = false;
    std::size_t lineno = 1;
    for(std::string line; std::getline(ifs, line); )
    {
        if(!inside)
        {
            ofs << line << '\n';
            ++lineno;
        }
        if(line == "//$PLACEMENT_BEGIN")
        {
            inside = true;
            for(auto& o: order)
            {
                auto& mod2 = modules.find(o)->second;
                ofs << "#line " << std::get<4>(mod2) << " \"extrasrc/functions/" << o << ".hh\"\n";
                lineno += 1;
                ofs << std::get<0>(mod2);
                lineno += std::get<3>(mod2);
            }
        }
        else if(line == "//$PLACEMENT_END")
        {
            ++lineno;
            ofs << "#line " << lineno << " \"extrasrc/fpaux.hh\"\n";
            ++lineno;
            ofs << line << '\n';
            inside = false;
        }
    }}
    return std::rename(tempfn.c_str(), outputfn.c_str());
}
