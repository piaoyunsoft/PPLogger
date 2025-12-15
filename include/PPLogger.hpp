/******************************************************************************
 * 文件名称: PPLogger.hpp
 * 文件描述: 轻量级跨平台日志系统 - 支持多级别彩色日志输出
 * 创建日期: 2025-12-15
 * 作    者: 飘云
 * 版    本: V1.0
 *
 * 功能特性:
 *   - 多级别日志支持 (Debug/Info/Warning/Error/Fatal)
 *   - 彩色控制台输出 (Windows原生/ANSI跨平台)
 *   - 文件日志输出
 *   - 线程安全设计
 *   - 接口隔离原则
 *   - 单例模式管理
 *
 * 使用示例:
 *   PPLOG_INFO("这是一条信息");
 *   PPLOG_ERROR("错误码: %d", errorCode);
 *
 *
 *****************************************************************************/

#pragma once

#include <iostream>
#include <Windows.h>
#include <memory>
#include <mutex>

 // ============================================================================
 // 日志级别枚举
 // ============================================================================

enum class LogLevel
{
	Debug = 0,    // 调试信息
	Info = 1,     // 普通信息
	Warning = 2,  // 警告信息
	Error = 3,    // 错误信息
	Fatal = 4     // 致命错误
};

/******************************************************************************
 * 名称: LogLevelToString
 * 描述: 将日志级别转换为字符串表示
 * 输入: level - 日志级别枚举值
 * 输出: 无
 * 返回: 日志级别的字符串表示
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
inline const char* LogLevelToString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Debug:   return "[DEBUG]";
	case LogLevel::Info:    return "[INFO]";
	case LogLevel::Warning: return "[WARN]";
	case LogLevel::Error:   return "[ERROR]";
	case LogLevel::Fatal:   return "[FATAL]";
	default:                return "[UNKNOWN]";
	}
}

// ============================================================================
// 颜色辅助类 - Windows 控制台颜色支持
// ============================================================================

/******************************************************************************
 * 名称: ConsoleColor
 * 描述: Windows 控制台颜色控制辅助类
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class ConsoleColor
{
public:
	// 颜色枚举（Windows 控制台颜色常量）
	enum Color
	{
		Black = 0,
		DarkBlue = FOREGROUND_BLUE,
		DarkGreen = FOREGROUND_GREEN,
		DarkCyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkRed = FOREGROUND_RED,
		DarkMagenta = FOREGROUND_RED | FOREGROUND_BLUE,
		DarkYellow = FOREGROUND_RED | FOREGROUND_GREEN,
		Gray = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkGray = FOREGROUND_INTENSITY,
		Blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		Magenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	/**************************************************************************
	 * 名称: SetColor
	 * 描述: 设置控制台文本颜色
	 * 输入: color - 颜色枚举值
	 * 作者: 飘云
	 **************************************************************************/
	static void SetColor(Color color)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
	}

	/**************************************************************************
	 * 名称: ResetColor
	 * 描述: 恢复控制台默认颜色
	 * 作者: 飘云
	 **************************************************************************/
	static void ResetColor()
	{
		SetColor(Gray); // 恢复默认颜色
	}

	/**************************************************************************
	 * 名称: GetColorForLogLevel
	 * 描述: 根据日志级别获取对应的颜色
	 * 输入: level - 日志级别
	 * 返回: 对应的颜色枚举值
	 * 作者: 飘云
	 **************************************************************************/
	static Color GetColorForLogLevel(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Debug:   return DarkGray;   // 深灰色
		case LogLevel::Info:    return Cyan;       // 青色
		case LogLevel::Warning: return Yellow;     // 黄色
		case LogLevel::Error:   return Red;        // 红色
		case LogLevel::Fatal:   return Magenta;    // 洋红色
		default:                return Gray;
		}
	}
};

/******************************************************************************
 * 名称: ColorScope
 * 描述: RAII 风格的颜色作用域类（自动恢复颜色）
 * 作者: 飘云
 * 日期: 2025-12-15
 *
 * 使用示例:
 *   {
 *       ColorScope scope(ConsoleColor::Red);
 *       std::cout << "这是红色文本" << std::endl;
 *   } // 自动恢复默认颜色
 *****************************************************************************/
class ColorScope
{
public:
	explicit ColorScope(ConsoleColor::Color color)
	{
		ConsoleColor::SetColor(color);
	}

	~ColorScope()
	{
		ConsoleColor::ResetColor();
	}

	// 禁止拷贝
	ColorScope(const ColorScope&) = delete;
	ColorScope& operator=(const ColorScope&) = delete;
};

// ============================================================================
// ANSI 颜色辅助类 - 跨平台支持
// ============================================================================

/******************************************************************************
 * 名称: AnsiColor
 * 描述: ANSI 转义序列颜色控制类（支持 Linux/macOS/Windows 10+）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class AnsiColor
{
public:
	// ANSI 转义序列
	static constexpr const char* Reset = "\033[0m";
	static constexpr const char* Black = "\033[30m";
	static constexpr const char* Red = "\033[31m";
	static constexpr const char* Green = "\033[32m";
	static constexpr const char* Yellow = "\033[33m";
	static constexpr const char* Blue = "\033[34m";
	static constexpr const char* Magenta = "\033[35m";
	static constexpr const char* Cyan = "\033[36m";
	static constexpr const char* White = "\033[37m";

	// 高亮颜色
	static constexpr const char* BrightBlack = "\033[90m";
	static constexpr const char* BrightRed = "\033[91m";
	static constexpr const char* BrightGreen = "\033[92m";
	static constexpr const char* BrightYellow = "\033[93m";
	static constexpr const char* BrightBlue = "\033[94m";
	static constexpr const char* BrightMagenta = "\033[95m";
	static constexpr const char* BrightCyan = "\033[96m";
	static constexpr const char* BrightWhite = "\033[97m";

	/**************************************************************************
	 * 名称: GetColorForLogLevel
	 * 描述: 根据日志级别获取对应的 ANSI 颜色代码
	 * 输入: level - 日志级别
	 * 返回: ANSI 颜色转义序列字符串
	 * 作者: 飘云
	 **************************************************************************/
	static const char* GetColorForLogLevel(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Debug:   return BrightBlack;   // 亮黑（灰色）
		case LogLevel::Info:    return BrightCyan;    // 亮青
		case LogLevel::Warning: return BrightYellow;  // 亮黄
		case LogLevel::Error:   return BrightRed;     // 亮红
		case LogLevel::Fatal:   return BrightMagenta; // 亮洋红
		default:                return Reset;
		}
	}

	/**************************************************************************
	 * 名称: EnableAnsiOnWindows
	 * 描述: 在 Windows 10+ 系统上启用 ANSI 转义序列支持
	 * 作者: 飘云
	 * 说明: Windows 10 1511 及以上版本支持 ANSI 转义序列
	 **************************************************************************/
	static void EnableAnsiOnWindows()
	{
#ifdef _WIN32
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE)
		{
			DWORD dwMode = 0;
			if (GetConsoleMode(hOut, &dwMode))
			{
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
			}
		}
#endif
	}
};

// ============================================================================
// 第一层：最小化接口 - 严格遵循接口隔离原则
// ============================================================================

/******************************************************************************
 * 名称: ILogger
 * 描述: 日志记录核心接口（单一职责：记录日志）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class ILogger
{
public:
	virtual ~ILogger() = default;

	/**************************************************************************
	 * 名称: Log
	 * 描述: 记录一条日志
	 * 输入: level   - 日志级别
	 *       message - 日志消息内容
	 * 作者: 飘云
	 **************************************************************************/
	virtual void Log(LogLevel level, const char* message) = 0;
};

/******************************************************************************
 * 名称: IInitializable
 * 描述: 可初始化接口（单一职责：初始化和关闭）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class IInitializable
{
public:
	virtual ~IInitializable() = default;

	/**************************************************************************
	 * 名称: Init
	 * 函述: 初始化日志器
	 * 输入: parameter - 初始化参数（如文件路径）
	 * 作者: 飘云
	 * 日期: 2025-12-15
	 **************************************************************************/
	virtual void Init(const char* parameter = nullptr) = 0;

	/**************************************************************************
	 * 名称: Close
	 * 描述: 关闭日志器，释放资源
	 * 作者: 飘云
	 **************************************************************************/
	virtual void Close() = 0;
};

/******************************************************************************
 * 名称: ILogLevelFilter
 * 描述: 日志级别过滤接口（单一职责：过滤日志级别）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class ILogLevelFilter
{
public:
	virtual ~ILogLevelFilter() = default;

	/**************************************************************************
	 * 名称: SetMinLevel
	 * 描述: 设置最小日志级别（低于此级别的日志不输出）
	 * 输入: level - 最小日志级别
	 * 作者: 飘云
	 **************************************************************************/
	virtual void SetMinLevel(LogLevel level) = 0;

	/**************************************************************************
	 * 名称: GetMinLevel
	 * 描述: 获取当前最小日志级别
	 * 返回: 最小日志级别
	 * 作者: 飘云
	 **************************************************************************/
	virtual LogLevel GetMinLevel() const = 0;
};

/******************************************************************************
 * 名称: IColorControl
 * 描述: 颜色控制接口（单一职责：控制彩色输出）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class IColorControl
{
public:
	virtual ~IColorControl() = default;

	/**************************************************************************
	 * 名称: SetColorEnabled
	 * 描述: 启用或禁用彩色输出
	 * 输入: enabled - true 启用，false 禁用
	 * 作者: 飘云
	 **************************************************************************/
	virtual void SetColorEnabled(bool enabled) = 0;

	/**************************************************************************
	 * 名称: IsColorEnabled
	 * 描述: 查询是否启用彩色输出
	 * 返回: true 已启用，false 已禁用
	 * 作者: 飘云
	 **************************************************************************/
	virtual bool IsColorEnabled() const = 0;
};

// ============================================================================
// 第二层：辅助类 - 提供便捷功能但不强制要求
// ============================================================================

/******************************************************************************
 * 名称: LoggerFormatter
 * 描述: 格式化日志辅助类（使用组合而非继承）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class LoggerFormatter
{
public:
	explicit LoggerFormatter(ILogger* logger) : m_logger(logger) {}

	void LogDebug(const char* message)
	{
		if (m_logger) m_logger->Log(LogLevel::Debug, message);
	}

	void LogInfo(const char* message)
	{
		if (m_logger) m_logger->Log(LogLevel::Info, message);
	}

	void LogWarning(const char* message)
	{
		if (m_logger) m_logger->Log(LogLevel::Warning, message);
	}

	void LogError(const char* message)
	{
		if (m_logger) m_logger->Log(LogLevel::Error, message);
	}

	void LogFatal(const char* message)
	{
		if (m_logger) m_logger->Log(LogLevel::Fatal, message);
	}

	template<typename... Args>
	void Log(LogLevel level, const char* format, Args... args)
	{
		if (m_logger)
		{
			char buffer[1024];
			wsprintfA(buffer, format, args...);
			m_logger->Log(level, buffer);
		}
	}

	template<typename... Args>
	void LogDebug(const char* format, Args... args)
	{
		Log(LogLevel::Debug, format, args...);
	}

	template<typename... Args>
	void LogInfo(const char* format, Args... args)
	{
		Log(LogLevel::Info, format, args...);
	}

	template<typename... Args>
	void LogWarning(const char* format, Args... args)
	{
		Log(LogLevel::Warning, format, args...);
	}

	template<typename... Args>
	void LogError(const char* format, Args... args)
	{
		Log(LogLevel::Error, format, args...);
	}

	template<typename... Args>
	void LogFatal(const char* format, Args... args)
	{
		Log(LogLevel::Fatal, format, args...);
	}

private:
	ILogger* m_logger;
};

// ============================================================================
// 第三层：具体实现 - 按需实现接口
// ============================================================================

/******************************************************************************
 * 名称: ConsoleLogger
 * 描述: 控制台日志实现（Windows 原生彩色支持）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class ConsoleLogger : public ILogger, public ILogLevelFilter, public IColorControl
{
private:
	LogLevel m_minLevel = LogLevel::Debug;
	bool m_enableColor = true;
	std::mutex m_mutex; // 线程安全

public:
	void Log(LogLevel level, const char* message) override
	{
		if (level < m_minLevel)
			return;

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_enableColor)
		{
			ColorScope colorScope(ConsoleColor::GetColorForLogLevel(level));
			std::cout << LogLevelToString(level) << " " << message << std::endl;
		}
		else
		{
			std::cout << LogLevelToString(level) << " " << message << std::endl;
		}
	}

	void SetMinLevel(LogLevel level) override
	{
		m_minLevel = level;
	}

	LogLevel GetMinLevel() const override
	{
		return m_minLevel;
	}

	void SetColorEnabled(bool enabled) override
	{
		m_enableColor = enabled;
	}

	bool IsColorEnabled() const override
	{
		return m_enableColor;
	}
};

/******************************************************************************
 * 名称: AnsiConsoleLogger
 * 描述: ANSI 彩色控制台日志实现（跨平台）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class AnsiConsoleLogger : public ILogger, public ILogLevelFilter, public IInitializable, public IColorControl
{
private:
	LogLevel m_minLevel = LogLevel::Debug;
	bool m_enableColor = true;
	std::mutex m_mutex;

public:
	void Init(const char* parameter = nullptr) override
	{
		AnsiColor::EnableAnsiOnWindows();
	}

	void Close() override {}

	void Log(LogLevel level, const char* message) override
	{
		if (level < m_minLevel)
			return;

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_enableColor)
		{
			std::cout << AnsiColor::GetColorForLogLevel(level)
				<< LogLevelToString(level) << " " << message
				<< AnsiColor::Reset << std::endl;
		}
		else
		{
			std::cout << LogLevelToString(level) << " " << message << std::endl;
		}
	}

	void SetMinLevel(LogLevel level) override
	{
		m_minLevel = level;
	}

	LogLevel GetMinLevel() const override
	{
		return m_minLevel;
	}

	void SetColorEnabled(bool enabled) override
	{
		m_enableColor = enabled;
	}

	bool IsColorEnabled() const override
	{
		return m_enableColor;
	}
};

/******************************************************************************
 * 名称: EnhancedColorConsoleLogger
 * 描述: 增强彩色控制台日志（分段着色：时间戳+级别+消息）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class EnhancedColorConsoleLogger : public ILogger, public ILogLevelFilter, public IColorControl
{
private:
	LogLevel m_minLevel = LogLevel::Debug;
	bool m_enableColor = true;
	std::mutex m_mutex;

public:
	void Log(LogLevel level, const char* message) override
	{
		if (level < m_minLevel)
			return;

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_enableColor)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			{
				ColorScope timeColor(ConsoleColor::DarkGray);
				printf("[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
			}

			{
				ColorScope levelColor(ConsoleColor::GetColorForLogLevel(level));
				std::cout << LogLevelToString(level) << " ";
			}

			{
				ColorScope msgColor(ConsoleColor::White);
				std::cout << message << std::endl;
			}
		}
		else
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			printf("[%02d:%02d:%02d] %s %s\n",
				st.wHour, st.wMinute, st.wSecond,
				LogLevelToString(level), message);
		}
	}

	void SetMinLevel(LogLevel level) override
	{
		m_minLevel = level;
	}

	LogLevel GetMinLevel() const override
	{
		return m_minLevel;
	}

	void SetColorEnabled(bool enabled) override
	{
		m_enableColor = enabled;
	}

	bool IsColorEnabled() const override
	{
		return m_enableColor;
	}
};

/******************************************************************************
 * 名称: FileLogger
 * 描述: 文件日志实现
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class FileLogger : public ILogger, public IInitializable, public ILogLevelFilter
{
private:
	HANDLE m_hFile = INVALID_HANDLE_VALUE;
	char m_filePath[MAX_PATH] = { 0 };
	LogLevel m_minLevel = LogLevel::Debug;
	std::mutex m_mutex;

public:
	~FileLogger() override
	{
		Close();
	}

	void Log(LogLevel level, const char* message) override
	{
		if (level < m_minLevel || m_hFile == INVALID_HANDLE_VALUE)
			return;

		std::lock_guard<std::mutex> lock(m_mutex);

		char buffer[1024];
		SYSTEMTIME st;
		GetLocalTime(&st);

		wsprintfA(buffer, "[%04d-%02d-%02d %02d:%02d:%02d] %s %s\r\n",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond,
			LogLevelToString(level), message);

		DWORD written = 0;
		WriteFile(m_hFile, buffer, lstrlenA(buffer), &written, NULL);
		FlushFileBuffers(m_hFile);
	}

	void Init(const char* logFilePath = nullptr) override
	{
		if (logFilePath)
		{
			lstrcpyA(m_filePath, logFilePath);
			m_hFile = CreateFileA(
				logFilePath,
				GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (m_hFile != INVALID_HANDLE_VALUE)
			{
				SetFilePointer(m_hFile, 0, NULL, FILE_END);
			}
		}
	}

	void Close() override
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	void SetMinLevel(LogLevel level) override
	{
		m_minLevel = level;
	}

	LogLevel GetMinLevel() const override
	{
		return m_minLevel;
	}
};

/******************************************************************************
 * 名称: NullLogger
 * 描述: 空日志实现（用于 Release 版本，不输出任何日志）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class NullLogger : public ILogger, public IInitializable, public ILogLevelFilter
{
public:
	void Log(LogLevel level, const char* message) override {}
	void Init(const char* parameter = nullptr) override {}
	void Close() override {}
	void SetMinLevel(LogLevel level) override {}
	LogLevel GetMinLevel() const override { return LogLevel::Fatal; }
};

/******************************************************************************
 * 名称: CompositeLogger
 * 描述: 复合日志实现（同时输出到控制台和文件）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class CompositeLogger : public ILogger, public IInitializable, public ILogLevelFilter, public IColorControl
{
private:
	std::unique_ptr<ILogger> m_consoleLogger;
	std::unique_ptr<ILogger> m_fileLogger;
	IInitializable* m_fileInitializable = nullptr;
	ILogLevelFilter* m_consoleFilter = nullptr;
	ILogLevelFilter* m_fileFilter = nullptr;
	IColorControl* m_colorControl = nullptr;
	LogLevel m_minLevel = LogLevel::Debug;

public:
	CompositeLogger()
	{
		auto console = std::make_unique<EnhancedColorConsoleLogger>();
		m_consoleFilter = console.get();
		m_colorControl = console.get();
		m_consoleLogger = std::move(console);

		auto file = std::make_unique<FileLogger>();
		m_fileInitializable = file.get();
		m_fileFilter = file.get();
		m_fileLogger = std::move(file);
	}

	void Log(LogLevel level, const char* message) override
	{
		if (level < m_minLevel)
			return;

		m_consoleLogger->Log(level, message);
		m_fileLogger->Log(level, message);
	}

	void Init(const char* parameter = nullptr) override
	{
		if (m_fileInitializable && parameter)
		{
			m_fileInitializable->Init(parameter);
		}
	}

	void Close() override
	{
		if (m_fileInitializable)
		{
			m_fileInitializable->Close();
		}
	}

	void SetMinLevel(LogLevel level) override
	{
		m_minLevel = level;
		if (m_consoleFilter) m_consoleFilter->SetMinLevel(level);
		if (m_fileFilter) m_fileFilter->SetMinLevel(level);
	}

	LogLevel GetMinLevel() const override
	{
		return m_minLevel;
	}

	void SetColorEnabled(bool enabled) override
	{
		if (m_colorControl)
		{
			m_colorControl->SetColorEnabled(enabled);
		}
	}

	bool IsColorEnabled() const override
	{
		return m_colorControl ? m_colorControl->IsColorEnabled() : false;
	}
};

// ============================================================================
// 第四层：外观模式 - 统一管理接口
// ============================================================================

/******************************************************************************
 * 名称: PPLogger
 * 描述: 日志管理器（单例模式 + 外观模式）
 * 作者: 飘云
 * 日期: 2025-12-15
 *****************************************************************************/
class PPLogger
{
public:
	static PPLogger& GetInstance()
	{
		static PPLogger instance;
		return instance;
	}

	void SetLogger(std::unique_ptr<ILogger> logger)
	{
		if (m_logger)
		{
			auto* initializable = dynamic_cast<IInitializable*>(m_logger.get());
			if (initializable)
			{
				initializable->Close();
			}
		}

		m_logger = std::move(logger);
		m_formatter = std::make_unique<LoggerFormatter>(m_logger.get());
	}

	ILogger* GetLogger() const
	{
		return m_logger.get();
	}

	LoggerFormatter* GetFormatter() const
	{
		return m_formatter.get();
	}

	void Init(const char* parameter = nullptr)
	{
		if (m_logger)
		{
			auto* initializable = dynamic_cast<IInitializable*>(m_logger.get());
			if (initializable)
			{
				initializable->Init(parameter);
			}
		}
	}

	void Close()
	{
		if (m_logger)
		{
			auto* initializable = dynamic_cast<IInitializable*>(m_logger.get());
			if (initializable)
			{
				initializable->Close();
			}
		}
	}

	void SetMinLevel(LogLevel level)
	{
		if (m_logger)
		{
			auto* filter = dynamic_cast<ILogLevelFilter*>(m_logger.get());
			if (filter)
			{
				filter->SetMinLevel(level);
			}
		}
	}

	void SetColorEnabled(bool enabled)
	{
		if (m_logger)
		{
			auto* colorControl = dynamic_cast<IColorControl*>(m_logger.get());
			if (colorControl)
			{
				colorControl->SetColorEnabled(enabled);
			}
		}
	}

	template<typename... Args>
	void Log(LogLevel level, const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->Log(level, format, args...);
		}
	}

	template<typename... Args>
	void LogDebug(const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->LogDebug(format, args...);
		}
	}

	template<typename... Args>
	void LogInfo(const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->LogInfo(format, args...);
		}
	}

	template<typename... Args>
	void LogWarning(const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->LogWarning(format, args...);
		}
	}

	template<typename... Args>
	void LogError(const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->LogError(format, args...);
		}
	}

	template<typename... Args>
	void LogFatal(const char* format, Args... args)
	{
		if (m_formatter)
		{
			m_formatter->LogFatal(format, args...);
		}
	}

	~PPLogger()
	{
		Close();
	}

private:
	PPLogger()
	{
#ifdef _DEBUG
		auto logger = std::make_unique<EnhancedColorConsoleLogger>();
		logger->SetMinLevel(LogLevel::Debug);
		SetLogger(std::move(logger));
#else
		SetLogger(std::make_unique<NullLogger>());
#endif
	}

	PPLogger(const PPLogger&) = delete;
	PPLogger& operator=(const PPLogger&) = delete;

	std::unique_ptr<ILogger> m_logger;
	std::unique_ptr<LoggerFormatter> m_formatter;
};

// ============================================================================
// 宏定义 - 保持简洁的API
// ============================================================================

// 在 Release 版本完全移除日志
#ifdef _DEBUG
#define PPLOG_ENABLED 1
#else
#define PPLOG_ENABLED 0
#endif

#define PP_LOGGER PPLogger::GetInstance()

#if PPLOG_ENABLED
#define PPLOG_DEBUG(format, ...)   PP_LOGGER.Log(LogLevel::Debug, format, ##__VA_ARGS__)
#define PPLOG_INFO(format, ...)    PP_LOGGER.Log(LogLevel::Info, format, ##__VA_ARGS__)
#define PPLOG_WARNING(format, ...) PP_LOGGER.Log(LogLevel::Warning, format, ##__VA_ARGS__)
#define PPLOG_WARN(format, ...)    PP_LOGGER.Log(LogLevel::Warning, format, ##__VA_ARGS__)
#define PPLOG_ERROR(format, ...)   PP_LOGGER.Log(LogLevel::Error, format, ##__VA_ARGS__)
#define PPLOG_FATAL(format, ...)   PP_LOGGER.Log(LogLevel::Fatal, format, ##__VA_ARGS__)
#else
	// Release 版本：完全移除，编译器会优化掉整个表达式
#define PPLOG_DEBUG(format, ...)   do { (void)0; } while(0)
#define PPLOG_INFO(format, ...)    do { (void)0; } while(0)
#define PPLOG_WARNING(format, ...) do { (void)0; } while(0)
#define PPLOG_WARN(format, ...)    do { (void)0; } while(0)
#define PPLOG_ERROR(format, ...)   do { (void)0; } while(0)
#define PPLOG_FATAL(format, ...)   do { (void)0; } while(0)
#endif
