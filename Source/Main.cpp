#include <JuceHeader.h>
#include "MainComponent.h"


class HAIFISCH_2Application : public JUCEApplication
{
public:
    HAIFISCH_2Application() {}

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;                                                                   //usuwa nasze okno
    }

    void systemRequestedQuit() override
    {
        quit();                                                                                 //Wywo³ywane, gdy apka prosi o zamkniêcie
                                                                                                //pozwól aplikacji dzia³aæ dalej, lub wywo³aj quit(), aby zamkn¹æ
    }

    void anotherInstanceStarted(const String& commandLine) override
    {
        //Gdy podczas dzia³ania tej aplikacji zostanie uruchomiona inna czêœæ aplikacji
    }

    class MainWindow : public DocumentWindow                                                 //Implementacja okna
    {
    public:
        MainWindow(String name) : DocumentWindow(name,
            Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
#endif

            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();                              //Wywo³ywane przy zamkniêciu okna
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(HAIFISCH_2Application)                                                 //Makro generuj¹ce main(), uruchamia apke