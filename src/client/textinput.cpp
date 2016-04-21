#include "textinput.hpp"
#include "../components/textwindow.hpp"
#include "clientdata.hpp"

#include <string.h>

namespace ClientEngine
{
    Console console;
    bool consoleActive = 0;
    
    void RunConsoleCommand(std::string command)
    {
        console.display->append_line(std::string("> ") + command);
        std::vector<std::string> arglist;
        std::string scraparg("");
        
        bool escape = false;
        bool encapsulate = false;
        for(auto character : command)
        {
            if ( character == '\\' and escape == false ) // character is '\'
                escape = true;
            else
            {
                if ( escape )
                {
                    switch ( character )
                    {
                    case 'n':
                        scraparg += '\n';
                        break;
                    case 't':
                        scraparg += '\t';
                        break;
                    case '\\':
                        scraparg += '\\';
                        break;
                    case '"':
                        scraparg += '\"';
                        break;
                    case ' ':
                        scraparg += ' ';
                        break;
                    default:
                        scraparg += '\\';
                        scraparg += character;
                    }
                    escape = false;
                }
                else
                {
                    switch ( character )
                    {
                    case '"':
                        encapsulate = !encapsulate;
                        if (!encapsulate)
                        {
                            arglist.push_back(scraparg);
                            scraparg = "";
                        }
                        break;
                    case ' ':
                        if(encapsulate)
                            scraparg += ' ';
                        else
                        {
                            arglist.push_back(scraparg);
                            scraparg = "";
                        }
                        break;
                    default:
                        scraparg += character;
                    }
                }
                    
                escape = false;
            }
        }
        if(strcmp(scraparg.data(), "") != 0)
            arglist.push_back(scraparg);
        
        if(arglist.size() < 1)
            return;
        std::string print;
        /* Example:
        if(strcmp(arglist[0].data(), "connect") == 0)
            print = Connect(arglist);
        */
        console.display->append_line(print);
    }
    
    Sys::chainreturn Hotkeys(SDL_Event event)
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_BACKQUOTE:
                consoleActive = !consoleActive;
                console.display->visible = consoleActive;
                console.input->visible   = consoleActive;
                if(consoleActive)
                {
                    SDL_StartTextInput();
                    console.input->line = "";
                }
                else
                {
                    SDL_StopTextInput();
                    console.input->line = "";
                }
                return Sys::CHAIN_FINISH;
            case SDLK_RETURN:
                RunConsoleCommand(console.input->line);
                console.input->line = "";
                // TODO: Commit command
                return Sys::CHAIN_FINISH;
            case SDLK_BACKSPACE:
                if(console.input->line.length() > 0)
                    console.input->line.erase(console.input->line.end()-1);
            }
            break;
        case SDL_TEXTINPUT:
            if(strcmp("`", event.text.text) != 0)
                console.input->line += event.text.text;
            break;
        }
        return Sys::CHAIN_CONTINUE;
    }
    
    Sys::chainreturn TextInput(SDL_Event event)
    {
        switch(event.type)
        {
        case 0:
            break;
        }
        return Sys::CHAIN_CONTINUE;
    }
}
