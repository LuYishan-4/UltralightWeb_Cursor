#include "lib/header/CursorFX.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <queue>
#include <atomic>


namespace
{



struct ConsoleCommand
{
    enum class Type
    {
        Enable,
        Disable,
        ChangeTheme,
        Quit,
        Unknown
    };

    Type type = Type::Unknown;
    std::string arg;
};



class CommandQueue
{

public:

    void push(ConsoleCommand cmd)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(cmd));
    }

    bool tryPop(ConsoleCommand& out)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if(queue_.empty())
            return false;

        out = std::move(queue_.front());
        queue_.pop();

        return true;
    }

private:

    std::mutex mutex_;
    std::queue<ConsoleCommand> queue_;

};


void printMenu()
{
    std::cout
        << "\n"
        << "==== CursorFX  ====\n"
        << "  1 = Enable\n"
        << "  2 = Disable\n"
        << "  3 = ChangeTheme\n"
        << "  q = Quit\n"
        << ">: "<<'\n';
      
}


ConsoleCommand parseLine(const std::string& line)
{
    ConsoleCommand cmd;

    if(line == "1")
    {
        cmd.type = ConsoleCommand::Type::Enable;
    }
    else if(line == "2")
    {
        cmd.type = ConsoleCommand::Type::Disable;
    }
    else if(line == "3")
    {
        std::cout << "input File path: " << std::flush;

        std::string path;
        std::getline(std::cin, path);

        cmd.type = ConsoleCommand::Type::ChangeTheme;
        cmd.arg = path;
    }
    else if(line == "q" || line == "Q")
    {
        cmd.type = ConsoleCommand::Type::Quit;
    }
    else
    {
        cmd.type = ConsoleCommand::Type::Unknown;
    }

    return cmd;
}



void inputThreadLoop(
    CommandQueue& queue,
    std::atomic<bool>& running
)
{
    while(running.load())
    {
        printMenu();

        std::string line;

        if(!std::getline(std::cin, line))
        {
            break;
        }

        ConsoleCommand cmd = parseLine(line);

        if(cmd.type == ConsoleCommand::Type::Unknown)
        {
            std::cout << "bruh\n";
            continue;
        }

        queue.push(cmd);

        if(cmd.type == ConsoleCommand::Type::Quit)
        {
            running.store(false);
            break;
        }
    }
}


}


int main(
    int argc,
    char** argv
)
{
    std::cout << "CursorFX start\n";

    CursorFX::CursorEngine engine;

    if(!engine.initialize())
    {
        std::cerr << "CursorFX initialize failed\n";
        return -1;
    }

    std::cout << "CursorFX running\n";

    CommandQueue commandQueue;
    std::atomic<bool> running{true};

    std::thread inputThread(
        inputThreadLoop,
        std::ref(commandQueue),
        std::ref(running)
    );

   
    while(running.load())
    {
        ConsoleCommand cmd;

        while(commandQueue.tryPop(cmd))
        {
            switch(cmd.type)
            {
                case ConsoleCommand::Type::Enable:
                    engine.enable(true);
                    std::cout << "\n[main] now running\n";
                    break;

                case ConsoleCommand::Type::Disable:
                    engine.enable(false);
                    std::cout << "\n[main] close now\n";
                    break;

                case ConsoleCommand::Type::ChangeTheme:
                    if(engine.setTheme(cmd.arg))
                    {
                        std::cout << "\n[main] :): " << cmd.arg << "\n";
                    }
                    else
                    {
                        std::cerr << "\n[main] Failed " << cmd.arg << "\n";
                    }
                    break;

                case ConsoleCommand::Type::Quit:
                    running.store(false);
                    break;

                default:
                    break;
            }
        }

        if(!running.load())
            break;

        engine.run();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "\nCiallo...\n";

    if(inputThread.joinable())
        inputThread.join();

    std::cout << "CursorFX exit\n";

    return 0;
}