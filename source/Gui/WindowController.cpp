#include "WindowController.h"

#include <GLFW/glfw3.h>

#include <boost/algorithm/string.hpp>

#include "Base/LoggingService.h"

#include "GlobalSettings.h"

namespace
{
    auto const WindowedMode = "window";
    auto const DesktopMode = "desktop";

    GLFWvidmode convert(std::string const& mode)
    {
        std::vector<std::string> modeParts;
        boost::split(modeParts, mode, [](char c) { return c == ' '; });

        CHECK(modeParts.size() == 6);

        GLFWvidmode result;
        result.width = std::stoi(modeParts.at(0));
        result.height = std::stoi(modeParts.at(1));
        result.redBits = std::stoi(modeParts.at(2));
        result.greenBits = std::stoi(modeParts.at(3));
        result.blueBits = std::stoi(modeParts.at(4));
        result.refreshRate = std::stoi(modeParts.at(5));
        return result;
    }

    std::string convert(GLFWvidmode const& vidmode)
    {
        std::vector<std::string> modeParts;
        modeParts.emplace_back(std::to_string(vidmode.width));
        modeParts.emplace_back(std::to_string(vidmode.height));
        modeParts.emplace_back(std::to_string(vidmode.redBits));
        modeParts.emplace_back(std::to_string(vidmode.greenBits));
        modeParts.emplace_back(std::to_string(vidmode.blueBits));
        modeParts.emplace_back(std::to_string(vidmode.refreshRate));

        return boost::join(modeParts, " ");
    }
}

_WindowController::_WindowController()
{
    _mode = GlobalSettings::getInstance().getStringState("settings.display.mode", DesktopMode);
    _sizeInWindowedMode.x = GlobalSettings::getInstance().getIntState("settings.display.window width", _sizeInWindowedMode.x);
    _sizeInWindowedMode.y = GlobalSettings::getInstance().getIntState("settings.display.window height", _sizeInWindowedMode.y);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    _windowData.mode = glfwGetVideoMode(primaryMonitor);

    _desktopVideoMode = new GLFWvidmode;
    *_desktopVideoMode = *_windowData.mode;

    _windowData.window = [&] {
        if (isWindowedMode()) {
            log(Priority::Important, "set windowed mode");
            _startupSize = _sizeInWindowedMode;
            return glfwCreateWindow(_sizeInWindowedMode.x, _sizeInWindowedMode.y, "alien", nullptr, nullptr);
        } else {
            log(Priority::Important, "set full screen mode");
            _startupSize = {_windowData.mode->width, _windowData.mode->height };
            return glfwCreateWindow(_windowData.mode->width, _windowData.mode->height, "alien", primaryMonitor, nullptr);
        }
    }();

    if (_windowData.window == nullptr) {
        throw std::runtime_error("Failed to create window.");
    }
    glfwMakeContextCurrent(_windowData.window);

    if (!isWindowedMode() && !isDesktopMode()) {
        auto userMode = getUserDefinedResolution();
        _startupSize = {userMode.width, userMode.height};
        log(Priority::Important, "switching to  " + createLogString(userMode));
        glfwSetWindowMonitor(
            _windowData.window, primaryMonitor, 0, 0, userMode.width, userMode.height, userMode.refreshRate);
    }
}

_WindowController::~_WindowController()
{
    delete _desktopVideoMode;
}

void _WindowController::shutdown()
{
    auto& settings = GlobalSettings::getInstance();
    settings.setStringState("settings.display.mode", _mode);

    if (isWindowedMode()) {
        updateWindowSize();
    }
    settings.setIntState("settings.display.window width", _sizeInWindowedMode.x);
    settings.setIntState("settings.display.window height", _sizeInWindowedMode.y);
}

auto _WindowController::getWindowData() const -> WindowData
{
    return _windowData;
}

bool _WindowController::isWindowedMode() const
{
    return _mode == WindowedMode;
}

bool _WindowController::isDesktopMode() const
{
    return _mode == DesktopMode;
}

GLFWvidmode _WindowController::getUserDefinedResolution() const
{
    return convert(_mode);
}

void _WindowController::setWindowedMode()
{
    setMode(WindowedMode);
}

void _WindowController::setDesktopMode()
{
    setMode(DesktopMode);
}

void _WindowController::setUserDefinedResolution(GLFWvidmode const& videoMode)
{
    setMode(convert(videoMode));
}

IntVector2D _WindowController::getStartupWindowSize() const
{
    return _startupSize;
}

std::string _WindowController::getMode() const
{
    return _mode;
}

void _WindowController::setMode(std::string const& mode)
{
    if (isWindowedMode()) {
        updateWindowSize();
    }

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

    if(mode == WindowedMode) {
        log(Priority::Important, "set windowed mode");
        GLFWvidmode const* desktopVideoMode = glfwGetVideoMode(primaryMonitor);
        glfwSetWindowMonitor(
            _windowData.window,
            nullptr,
            0,
            0,
            _sizeInWindowedMode.x,
            _sizeInWindowedMode.y,
            desktopVideoMode->refreshRate);
    } else if(mode == DesktopMode) {
        log(
            Priority::Important, "set full screen mode with " + createLogString(*_desktopVideoMode));
        glfwSetWindowMonitor(
            _windowData.window,
            primaryMonitor,
            0,
            0,
            _desktopVideoMode->width,
            _desktopVideoMode->height,
            _desktopVideoMode->refreshRate);
    } else {
        auto userMode = convert(mode);
        log(Priority::Important, "set full screen mode with " + createLogString(userMode));
        glfwSetWindowMonitor(
            _windowData.window, primaryMonitor, 0, 0, userMode.width, userMode.height, userMode.refreshRate);
    }
    _mode = mode;
}

void _WindowController::updateWindowSize()
{
    glfwGetWindowSize(_windowData.window, &_sizeInWindowedMode.x, &_sizeInWindowedMode.y);
}

std::string _WindowController::createLogString(GLFWvidmode const& videoMode) const
{
    std::stringstream ss;
    ss << videoMode.width << " x " << videoMode.height << " @ " << videoMode.refreshRate << "Hz";
    return ss.str();
}
