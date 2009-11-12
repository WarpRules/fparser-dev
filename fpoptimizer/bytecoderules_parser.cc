#include <ctype.h>
#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <set>

namespace
{
    struct Operation
    {
        enum { Opcode, Immed } type;
        std::string result;
    };
    struct MatchWith
    {
    };
    struct Match
    {
        enum { FixedOpcode, Immed, AnyOpcode } type;
        std::string name;         // opcode name such as cInt, or holder name such as x or X
        std::string condition;    // condition that applies when Immed or AnyOpcode

        bool operator==(const Match& b) const
        {
            return type==b.type
                && name==b.name
                && condition==b.condition;
        }
        std::vector<Operation> operations;
        bool has_operations;
    };
    struct Node
    {
        Match opcode;
        std::vector<Node*> predecessors;
    };

    Node global_head;

    std::string Indent(size_t n)
    {
        return std::string(n, ' ');
    }

    std::string Bexpr(size_t pos)
    {
        if(pos == 0) return "opcode";
        if(pos == 1) return "data->ByteCode.back()";
        std::ostringstream tmp;
        tmp << "data->ByteCode[blen - " << pos << "]";
        return tmp.str();
    }

    std::string Iexpr(size_t pos)
    {
        if(pos == 0) return "data->Immed.back()";
        std::ostringstream tmp;
        tmp << "data->Immed[ilen - " << (pos+1) << "]";
        return tmp.str();
    }

    std::string BexprName(size_t pos)
    {
        if(pos == 0) return "opcode";
        std::ostringstream tmp;
        tmp << "op_" << pos;
        return tmp.str();
    }

    bool HasHandlingFor(const std::string& opcode)
    {
        if(!(opcode[0] == 'c' && isupper(opcode[1])))
            return true;
        for(size_t b=0; b<global_head.predecessors.size(); ++b)
            if(global_head.predecessors[b]->opcode.type == Match::FixedOpcode
            && global_head.predecessors[b]->opcode.name == opcode)
                return true;
        return false;
    }

    bool SynthOperations(
        size_t indent, std::ostream& out,
        const std::vector<Match>& so_far,
        const std::vector<Operation>& operations,
        size_t b_used,
        size_t i_used)
    {
        if(!operations.empty() && operations[0].result == "DO_POWI")
        {
            out << Indent(indent) << "if(TryCompilePowi(" << so_far.back().name << "))\n";
            out << Indent(indent) << "    return;\n";
            return false;
        }

        int n_b_exist  = (int)(b_used-1);
        int n_i_exist  = (int)(i_used  );

        int b_offset = n_b_exist;
        int i_offset = n_i_exist;

        out << Indent(indent) << "FP_TRACE_BYTECODE_OPTIMIZATION(\"";
        for(size_t a=so_far.size(); a-- > 0; )
        {
            if(a+1 != so_far.size()) out << ' ';
            out << so_far[a].name;
            if(!so_far[a].condition.empty())
                out << '[' << so_far[a].condition << ']';
        }
        out << "\", \"";
        for(size_t a=0; a<operations.size(); ++a)
        {
            if(a > 0) out << ' ';
            if(operations[a].type == Operation::Immed) out << '[';
            out << operations[a].result;
            if(operations[a].type == Operation::Immed) out << ']';
        }
        out << "\");\n";
        for(size_t a=0; a<operations.size(); ++a)
        {
            std::string opcode = operations[a].result;
            if(operations[a].type == Operation::Immed)
            {
                if(i_offset > 0)
                {
                    out << Indent(indent) << Iexpr(i_offset-1) << " = " << opcode << ";\n";
                    --i_offset;
                }
                else
                    out << Indent(indent) << "data->Immed.push_back(" << opcode << ");\n";
                opcode = "cImmed";
            }

            bool redundant = false;
            if(b_offset > 0)
            {
                const Match& m = so_far[b_offset];
                if(opcode == (m.type == Match::Immed ? "cImmed" : m.name))
                {
                    redundant = true;
                }
            }

            if(!redundant && HasHandlingFor(opcode))
            {
                if(b_offset == 1)
                    out << Indent(indent) << "data->ByteCode.pop_back();\n";
                else if(b_offset > 0)
                     out << Indent(indent) << "data->ByteCode.resize(blen - " << b_offset << ");\n";
                if(i_offset == 1)
                    out << Indent(indent) << "data->Immed.pop_back();\n";
                else if(i_offset > 0)
                    out << Indent(indent) << "data->Immed.resize(ilen - " << i_offset << ");\n";
                out << Indent(indent) << "AddFunctionOpcode(" << opcode << ");\n";
                b_offset = 0;
                i_offset = 0;
            }
            else
            {
                if(b_offset > 0)
                {
                    if(redundant)
                    {
                        out << Indent(indent) << "/* " << Bexpr(b_offset) << " = " << opcode << "; */";
                        out << " // redundant, matches " << so_far[b_offset].name;
                        out << " @ " << (b_offset) << "\n";
                    }
                    else
                        out << Indent(indent) << Bexpr(b_offset) << " = " << opcode << ";\n";
                    --b_offset;
                }
                else
                    out << Indent(indent) << "data->ByteCode.push_back(" << opcode << ");\n";
            }
        }
        if(b_offset == 1)
            out << Indent(indent) << "data->ByteCode.pop_back();\n";
        else if(b_offset > 0)
             out << Indent(indent) << "data->ByteCode.resize(blen - " << b_offset << ");\n";
        if(i_offset == 1)
            out << Indent(indent) << "data->Immed.pop_back();\n";
        else if(i_offset > 0)
            out << Indent(indent) << "data->Immed.resize(ilen - " << i_offset << ");\n";
        return true;
    }

    enum { mode_children = 1, mode_operations = 2 };
    void Generate(
        const Node& head,
        const std::vector<Match>& so_far,
        size_t indent,
        std::ostream& out,
        size_t b_used,
        size_t i_used,
        int mode = mode_children+mode_operations)
    {
        if(!head.predecessors.empty() && (mode & mode_children))
        {
            std::string last_op_name = BexprName(b_used);
            if(last_op_name != "opcode")
                out << Indent(indent) << "unsigned " << last_op_name << " = " << Bexpr(b_used) << ";\n";
            out << Indent(indent) << "switch(" << last_op_name << ")\n";
            out << Indent(indent) << "{\n";
            for(size_t a=0; a<head.predecessors.size(); ++a)
            {
                const Node& n = *head.predecessors[a];
                if(n.opcode.type == Match::FixedOpcode)
                {
                    out << Indent(indent) << "case " << n.opcode.name << ":\n";
                    out << Indent(indent) << "  {\n";
                    std::vector<Match> ref(so_far);
                    ref.push_back(n.opcode);
                    Generate(n, ref, indent+4, out, b_used+1, i_used);
                    out << Indent(indent) << "  }\n";
                    out << Indent(indent) << "  break;\n";
                }
            }
            bool first_immed = true;
            std::set<std::string> immed_labels;
            for(int round=0; round<4; ++round)
                for(size_t a=0; a<head.predecessors.size(); ++a)
                {
                    const Node& n = *head.predecessors[a];
                    if(n.opcode.type == Match::Immed)
                    {
                        if(round < 2  && n.opcode.has_operations) continue;
                        if(round >= 2 && !n.opcode.has_operations) continue;
                        if((round & 1) != !!n.opcode.condition.empty()) continue;
                        if(first_immed)
                        {
                            out << Indent(indent) << "case cImmed:\n";
                            first_immed = false;
                        }
                        //out << Indent(indent) << "  /* round " << round << " a = " << a << " */\n";
                        std::set<std::string>::iterator i = immed_labels.lower_bound(n.opcode.name);
                        if(i == immed_labels.end() || *i != n.opcode.name)
                        {
                            if(immed_labels.empty())
                                out << Indent(indent) << "  {\n";
                            out << Indent(indent) << "    double " << n.opcode.name << " = " << Iexpr(i_used) << ";\n";
                            immed_labels.insert(i, n.opcode.name);
                        }
                        std::vector<Match> ref(so_far);
                        ref.push_back(n.opcode);
                        if(n.opcode.condition.empty())
                            Generate(n, ref, indent+4, out, b_used+1, i_used+1, round>=2?mode_operations:mode_children);
                        else
                        {
                            out << Indent(indent) << "    if(" << n.opcode.condition << ")\n";
                            out << Indent(indent) << "    {\n";
                            Generate(n, ref, indent+8, out, b_used+1, i_used+1, round>=2?mode_operations:mode_children);
                            out << Indent(indent) << "    }\n";
                        }
                    }
                }
            if(!first_immed)
            {
                out << Indent(indent) << "    break;\n";
                if(!immed_labels.empty())
                    out << Indent(indent) << "  }\n";
            }

            bool first_anyopcode = true;
            std::set<std::string> opcode_labels;
            for(int round=0; round<4; ++round)
                for(size_t a=0; a<head.predecessors.size(); ++a)
                {
                    const Node& n = *head.predecessors[a];
                    if(n.opcode.type == Match::AnyOpcode)
                    {
                        if(round < 2  && n.opcode.has_operations) continue;
                        if(round >= 2 && !n.opcode.has_operations) continue;
                        if((round & 1) != !!n.opcode.condition.empty()) continue;
                        if(first_anyopcode)
                        {
                            if(first_immed)
                            {
                                out << Indent(indent) << "case cImmed: break;\n";
                            }
                            out << Indent(indent) << "default:\n";
                            first_anyopcode = false;
                        }
                        //out << Indent(indent) << "  /* round " << round << " a = " << a << " */\n";
                        std::set<std::string>::iterator i = opcode_labels.lower_bound(n.opcode.name);
                        if(i == opcode_labels.end() || *i != n.opcode.name)
                        {
                            out << Indent(indent) << "    unsigned " << n.opcode.name << " = " << last_op_name << "; " << n.opcode.name << "=" << n.opcode.name << ";\n";
                            opcode_labels.insert(i, n.opcode.name);
                        }
                        std::vector<Match> ref(so_far);
                        ref.push_back(n.opcode);
                        if(n.opcode.condition.empty())
                            Generate(n, ref, indent+4, out, b_used+1, i_used, round>=2?mode_operations:mode_children);
                        else
                        {
                            out << Indent(indent) << "    if(" << n.opcode.condition << ")\n";
                            out << Indent(indent) << "    {\n";
                            Generate(n, ref, indent+8, out, b_used+1, i_used, round>=2?mode_operations:mode_children);
                            out << Indent(indent) << "    }\n";
                        }
                    }
                }
            out << Indent(indent) << "}\n";
        }
        if(head.opcode.has_operations && (mode & mode_operations))
        {
            /*if(!head.predecessors.empty())
                std::cout << Indent(indent) << "/""* NOTE: POSSIBLY AMBIGIOUS *""/\n";*/
            if(SynthOperations(indent,out, so_far, head.opcode.operations, b_used, i_used))
                out << Indent(indent) << "return;\n";
        }
    }

    void Generate(std::ostream& out)
    {
        out << "#define FP_TRACE_BYTECODE_OPTIMIZATION(from,to)\n";
        out << "//#define FP_TRACE_BYTECODE_OPTIMIZATION(from,to) std::cout << \"Changing \\\"\" from \"\\\"\\n    into\\\"\" to \"\\\"\\n\"\n";
        out << "inline void FunctionParser::AddFunctionOpcode(unsigned opcode)\n"
               "{\n"
               "    size_t blen = data->ByteCode.size();\n"
               "    size_t ilen = data->Immed.size();\n";
        Generate(global_head, std::vector<Match>(), 4, out, 0,0);
        out << "    data->ByteCode.push_back(opcode);\n"
               "}\n";
    }

    void Parse()
    {
        for(;;)
        {
            char Buf[2048];
            if(!std::fgets(Buf, sizeof(Buf), stdin)) break;
            char* bufptr = Buf;
            while(*bufptr == ' ' || *bufptr == '\t') ++bufptr;
            if(*bufptr == '#' || *bufptr == '\r' || *bufptr == '\n') continue;

            std::vector<Match> sequence;
            for(;;)
            {
                Match m;
                m.has_operations = false;
                while(*bufptr == ' ' || *bufptr == '\t') ++bufptr;
                if(*bufptr == '-' && bufptr[1] == '>') break;
                while(isalnum(*bufptr)) m.name += *bufptr++;
                while(*bufptr == ' ' || *bufptr == '\t') ++bufptr;
                if(*bufptr == '[')
                {
                    size_t balance = 0; ++bufptr;
                    while(*bufptr != ']' || balance != 0)
                    {
                        if(*bufptr == '\r' || *bufptr == '\n') break;
                        if(*bufptr == '[') ++balance;
                        if(*bufptr == ']') --balance;
                        m.condition += *bufptr++;
                    }
                    if(*bufptr == ']') ++bufptr;
                }
                if(m.name[0] == 'c' && m.name.size() > 1)
                    m.type = Match::FixedOpcode;
                else if(isupper(m.name[0]))
                    m.type = Match::AnyOpcode;
                else
                    m.type = Match::Immed;

                sequence.push_back(m);
            }

            Node* head = &global_head;
            for(size_t b=sequence.size(); b-->0; )
            {
                const Match& m = sequence[b];
                bool dup = false;
                for(size_t a=0; a< head->predecessors.size(); ++a)
                {
                    if(m == head->predecessors[a]->opcode)
                    {
                        head = head->predecessors[a];
                        dup = true;
                        break;
                    }
                }
                if(!dup)
                {
                    Node* newhead = new Node;
                    newhead->opcode = m;
                    head->predecessors.push_back(newhead);
                    head = newhead;
                }
            }

            if(*bufptr == '-' && bufptr[1] == '>')
            {
                head->opcode.has_operations = true;
                bufptr += 2;
                for(;;)
                {
                    while(*bufptr == ' ' || *bufptr == '\t') ++bufptr;
                    if(*bufptr == '#' || *bufptr == '\r' || *bufptr == '\n') break;
                    Operation op;
                    if(*bufptr == '[')
                    {
                        size_t balance = 0; ++bufptr;
                        while(*bufptr != ']' || balance != 0)
                        {
                            if(*bufptr == '\r' || *bufptr == '\n') break;
                            if(*bufptr == '[') ++balance;
                            if(*bufptr == ']') --balance;
                            op.result += *bufptr++;
                        }
                        if(*bufptr == ']') ++bufptr;
                        op.type = Operation::Immed;
                    }
                    else
                    {
                        while(isalnum(*bufptr))
                            op.result += *bufptr++;
                        op.type = Operation::Opcode;
                    }
                    head->opcode.operations.push_back(op);
                }
            }
        }
    }
}

int main()
{
    Parse();
    Generate(std::cout);
    return 0;
}
