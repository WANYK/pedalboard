#include <JuceHeader.h>
#include "MainComponent.h"

class HAIFISCH_2Application  : public JUCEApplication
{
public:
    HAIFISCH_2Application() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    void initialise (const String& commandLine) override
    {
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;                                                                   //usuwa nasze okno
    }

    void systemRequestedQuit() override
    {
        quit();                                                                                 //Wywo�ywane, gdy apka prosi o zamkni�cie
                                                                                                //pozw�l aplikacji dzia�a� dalej, lub wywo�aj quit(), aby zamkn��
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
                                                                                                //Gdy podczas dzia�ania tej aplikacji zostanie uruchomiona inna cz�� aplikacji
    }

    class MainWindow    : public DocumentWindow                                                 //Implementacja okna
    {
    public:
        MainWindow (String name)  : DocumentWindow (name,
                                                    Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                    DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();                              //Wywo�ywane przy zamkni�ciu okna
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (HAIFISCH_2Application)                                                 //Makro generuj�ce main(), uruchamia apke
