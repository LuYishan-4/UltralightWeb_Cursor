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
        << "==== CursorFX 控制台 ====\n"
        << "  1 = 開啟特效\n"
        << "  2 = 關閉特效\n"
        << "  3 = 更換 HTML 主題\n"
        << "  q = 離開程式\n"
        << "請輸入指令: "
        << std::flush;
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
        std::cout << "請輸入 HTML 檔案路徑: " << std::flush;

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
            // stdin 關閉（例如被導向 /dev/null），避免忙迴圈
            break;
        }

        ConsoleCommand cmd = parseLine(line);

        if(cmd.type == ConsoleCommand::Type::Unknown)
        {
            std::cout << "無效指令，請重新輸入\n";
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

    // 輸入 thread：只負責讀終端、丟指令，不直接碰 Ultralight/Wayland
    std::thread inputThread(
        inputThreadLoop,
        std::ref(commandQueue),
        std::ref(running)
    );

    /*
        主迴圈

        注意：
        Ultralight Renderer::Update()
        Renderer::Render()

        必須固定同一個 thread
    */
    while(running.load())
    {
        // 每個 frame 檢查一次有沒有新指令，全部處理完再繼續渲染
        ConsoleCommand cmd;

        while(commandQueue.tryPop(cmd))
        {
            switch(cmd.type)
            {
                case ConsoleCommand::Type::Enable:
                    engine.enable(true);
                    std::cout << "\n[main] 特效已開啟\n";
                    break;

                case ConsoleCommand::Type::Disable:
                    engine.enable(false);
                    std::cout << "\n[main] 特效已關閉\n";
                    break;

                case ConsoleCommand::Type::ChangeTheme:
                    if(engine.setTheme(cmd.arg))
                    {
                        std::cout << "\n[main] 已切換 HTML: " << cmd.arg << "\n";
                    }
                    else
                    {
                        std::cerr << "\n[main] 切換 HTML 失敗: " << cmd.arg << "\n";
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

        // 渲染 cursor
        engine.run();

        // 60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "\nCursorFX 準備結束...\n";

    if(inputThread.joinable())
        inputThread.join();

    std::cout << "CursorFX exit\n";

    return 0;
}
