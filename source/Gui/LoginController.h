#pragma once

#include "Base/Singleton.h"
#include "Definitions.h"
#include "PersisterInterface/PersisterFacade.h"

class LoginController
{
    MAKE_SINGLETON(LoginController);
public:

    void init(SimulationController const& simController, PersisterFacade const& persisterFacade, ActivateUserDialog const& activateUserDialog, BrowserWindow const& browserWindow);
    void shutdown();

    void onLogin();

    void process();

    void saveSettings();

    bool shareGpuInfo() const;
    void setShareGpuInfo(bool value);

    bool isRemember() const;
    void setRemember(bool value);

    std::string const& getUserName() const;
    void setUserName(std::string const& value);

    std::string const& getPassword() const;
    void setPassword(std::string const& value);

    UserInfo getUserInfo();

private:
    SimulationController _simController; 
    PersisterFacade _persisterFacade;
    ActivateUserDialog _activateUserDialog;
    BrowserWindow _browserWindow;

    TaskProcessor _taskProcessor;

    bool _shareGpuInfo = true;
    bool _remember = true;
    std::string _userName;
    std::string _password;
};
