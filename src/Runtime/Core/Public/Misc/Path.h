
#pragma once

#include <string>
#include <vector>
#include "CoreUtils.h"
namespace CarrotToy {

// Small Path utility used by the engine. Provides basic string/path helpers
// (endsWith, startsWith, contains), normalization (convert backslashes to
// forward slashes, remove duplicate slashes), extension/filename helpers,
// and simple UTF conversions (std::string <-> std::wstring).
class CORE_API Path {
public:
	// String conversions
	static std::wstring toWString(const std::string& s);
	static std::string toString(const std::wstring& ws);

	// Checks
	static bool endsWith(const std::string& s, const std::string& suffix, bool caseSensitive = false);
	static bool startsWith(const std::string& s, const std::string& prefix, bool caseSensitive = false);
	static bool contains(const std::string& s, const std::string& sub, bool caseSensitive = false);

	// Normalization: convert backslashes to '/', optionally remove trailing slash.
	static std::string normalize(const std::string& p, bool removeTrailingSlash = false);

	// Extension and filename helpers
	static std::string getExtension(const std::string& path, bool includeDot = true);
	static std::string getFilename(const std::string& path);
	static std::string getBaseFilename(const std::string& path, bool removeExtension = true);
	static std::string getPath(const std::string& path);
	static std::string changeExtension(const std::string& path, const std::string& newExt);
	static std::string removeExtension(const std::string& path);

	// Special helper: if path ends with .spv, remove it and ensure the remaining
	// filename ends with one of the recognized stage suffixes (e.g. .vs/.vert or .ps/.frag).
	// If not, append the provided stageExt (like ".vs" or ".ps").
	static std::string stripSpvAndEnsureStage(const std::string& path, const std::string& stageExt);

	// Split by path separator (normalized '/')
	static std::vector<std::string> split(const std::string& path, char sep = '/');

	// Directory helpers (lightweight replacements for Unreal's FPaths)
	// LaunchDir: the current working directory where the app was launched
	static std::string LaunchDir();
	// ProjectDir: the root directory of the project (defaults to LaunchDir)
	static std::string ProjectDir();
	// ShaderWorkingDir: where project shader sources live (ProjectDir + "/shaders")
	static std::string ShaderWorkingDir();

	// Initialization API - call once at startup to set canonical dirs.
	static void SetLaunchDir(const std::string& dir);
	static void SetProjectDir(const std::string& dir);
	// Initialize from command line args and environment. argc/argv optional.
	static void InitFromCmdLineAndEnv(int argc = 0, const char** argv = nullptr);
};

} // namespace CarrotToy
