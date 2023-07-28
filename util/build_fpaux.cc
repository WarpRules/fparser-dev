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
    std::map<std::string/*name*/,
        std::tuple<std::string/*contents*/,
                   std::set<std::string>,/*dependencies*/
                   unsigned/*color*/,
                   unsigned/*line count*/
                 >> modules;
    for(const auto& fn: inputs)
    {
        std::ifstream ifs(fn);
        std::string module_name = fn;
        std::size_t p = module_name.rfind('/');
        if(p != module_name.npos) module_name.erase(0, p+1);
        p = module_name.find('.');
        if(p != module_name.npos) module_name.erase(p);
        //std::cerr << "Module: " << module_name << '\n';

        auto& mod = modules[module_name];
        std::string& contents = std::get<0>(mod);
        auto&    dependencies = std::get<1>(mod);
        unsigned& linecount   = std::get<3>(mod);

        for(std::string line; std::getline(ifs, line); )
        {
            if(line.substr(0, 8) == "//$DEP: ")
                dependencies.insert(line.substr(8));
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
        if(color == 1)
        {
            std::cerr << "Error: Cyclical dependency to " << from << '\n';
            for(auto& o: order) std::cerr << "- prior: " << o << '\n';
            return; // ERROR: loop detected
        }
        if(color == 2) return; // ok
        color = 1;
        for(auto& dep: deps) dfs(dep);
        order.push_back(from);
        color = 2;
    };
    for(auto& m: modules)
        if(std::get<2>(m.second) == 0)
            dfs(m.first);

    {std::ifstream ifs(outputfn);
    std::ofstream ofs(outputfn + ".new");
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
                ofs << "#line 1 \"extrasrc/functions/" << o << ".hh\"\n";
                lineno += 1;
                auto& mod2 = modules.find(o)->second;
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
    return std::rename((outputfn + ".new").c_str(), outputfn.c_str());
}
