#include "Misc/Path.h"

#include <algorithm>
#include <locale>
#include <codecvt>
#include <filesystem>
#include "CoreUtils.h"
namespace CarrotToy {

// -- helpers --------------------------------------------------------------
static std::string toLowerCopy(const std::string& s) {
	std::string out = s;
	std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return std::tolower(c); });
	return out;
}

static bool compareEndsWith(const std::string& s, const std::string& suffix, bool caseSensitive) {
	if (s.size() < suffix.size()) return false;
	if (caseSensitive) return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
	return toLowerCopy(s).compare(s.size() - suffix.size(), suffix.size(), toLowerCopy(suffix)) == 0;
}

static bool compareStartsWith(const std::string& s, const std::string& pref, bool caseSensitive) {
	if (s.size() < pref.size()) return false;
	if (caseSensitive) return s.compare(0, pref.size(), pref) == 0;
	return toLowerCopy(s).compare(0, pref.size(), toLowerCopy(pref)) == 0;
}

// -- Path implementation -------------------------------------------------

// cached values (set during initialization)
static std::string g_launchDir;
static std::string g_projectDir;


std::wstring Path::toWString(const std::string& s) {
	if (s.empty()) return {};
	// Use portable converter (std::wstring_convert). Note: deprecated in C++17 but
	// widely available and acceptable for small utilities; replace with a
	// platform-specific converter if you prefer.
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(s);
}

std::string Path::toString(const std::wstring& ws) {
	if (ws.empty()) return {};
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(ws);
}

bool Path::endsWith(const std::string& s, const std::string& suffix, bool caseSensitive) {
	return compareEndsWith(s, suffix, caseSensitive);
}

bool Path::startsWith(const std::string& s, const std::string& prefix, bool caseSensitive) {
	return compareStartsWith(s, prefix, caseSensitive);
}

bool Path::contains(const std::string& s, const std::string& sub, bool caseSensitive) {
	if (sub.empty()) return true;
	if (caseSensitive) return s.find(sub) != std::string::npos;
	auto ls = toLowerCopy(s);
	auto lsub = toLowerCopy(sub);
	return ls.find(lsub) != std::string::npos;
}

std::string Path::normalize(const std::string& p, bool removeTrailingSlash) {
	std::string out;
	out.reserve(p.size());
	// replace backslashes with '/', collapse multiple '/' into one
	bool lastWasSlash = false;
	for (unsigned char c : p) {
		char ch = (c == '\\') ? '/' : (char)c;
		if (ch == '/') {
			if (lastWasSlash) continue;
			lastWasSlash = true;
		} else {
			lastWasSlash = false;
		}
		out.push_back(ch);
	}
	if (removeTrailingSlash && out.size() > 1 && out.back() == '/') out.pop_back();
	return out;
}

std::string Path::getExtension(const std::string& path, bool includeDot) {
	// find last dot after last slash
	size_t lastSlash = path.find_last_of("/\\");
	size_t lastDot = path.find_last_of('.');
	if (lastDot == std::string::npos) return std::string();
	if (lastSlash != std::string::npos && lastDot < lastSlash) return std::string();
	if (includeDot) return path.substr(lastDot);
	return path.substr(lastDot + 1);
}

std::string Path::getFilename(const std::string& path) {
	size_t lastSlash = path.find_last_of("/\\");
	if (lastSlash == std::string::npos) return path;
	return path.substr(lastSlash + 1);
}

std::string Path::getBaseFilename(const std::string& path, bool removeExtension) {
	auto name = getFilename(path);
	if (!removeExtension) return name;
	size_t lastDot = name.find_last_of('.');
	if (lastDot == std::string::npos) return name;
	return name.substr(0, lastDot);
}

std::string Path::getPath(const std::string& path) {
	size_t lastSlash = path.find_last_of("/\\");
	if (lastSlash == std::string::npos) return std::string();
	return path.substr(0, lastSlash);
}

std::string Path::changeExtension(const std::string& path, const std::string& newExt) {
	std::string base = removeExtension(path);
	if (newExt.empty()) return base;
	if (newExt.front() != '.') return base + "." + newExt;
	return base + newExt;
}

std::string Path::removeExtension(const std::string& path) {
	size_t lastSlash = path.find_last_of("/\\");
	size_t lastDot = path.find_last_of('.');
	if (lastDot == std::string::npos) return path;
	if (lastSlash != std::string::npos && lastDot < lastSlash) return path;
	return path.substr(0, lastDot);
}

std::string Path::stripSpvAndEnsureStage(const std::string& path, const std::string& stageExt) {
	// normalize stageExt to start with '.'
	std::string ext = stageExt;
	if (!ext.empty() && ext[0] != '.') ext.insert(ext.begin(), '.');

	std::string p = path;
	// if ends with .spv remove
	if (endsWith(p, ".spv", false)) {
		p = p.substr(0, p.size() - 4);
	}

	std::string lower = toLowerCopy(p);
	// if ends with a known stage extension, just return
	if (endsWith(lower, ".vs", false) || endsWith(lower, ".vert", false) ||
		endsWith(lower, ".ps", false) || endsWith(lower, ".frag", false)) {
		return p;
	}

	// otherwise append provided stageExt
	return p + ext;
}

std::vector<std::string> Path::split(const std::string& path, char sep) {
	std::vector<std::string> parts;
	std::string cur;
	for (char c : path) {
		char ch = c;
		if (ch == '\\') ch = '/';
		if (ch == sep) {
			if (!cur.empty()) parts.push_back(cur);
			cur.clear();
		} else {
			cur.push_back(ch);
		}
	}
	if (!cur.empty()) parts.push_back(cur);
	return parts;
}

std::string Path::LaunchDir() {
	if (!g_launchDir.empty()) return g_launchDir;
	try {
		return normalize(std::filesystem::current_path().string(), true);
	} catch (...) {
		return std::string();
	}
}

std::string Path::ProjectDir() {
	if (!g_projectDir.empty()) return g_projectDir;
	const char* env = std::getenv("CARROTTOY_PROJECT_DIR");
	if (env && env[0]) return normalize(std::string(env), true);
	return LaunchDir();
}

std::string Path::ShaderWorkingDir() {
	std::string proj = ProjectDir();
	if (proj.empty()) proj = LaunchDir();
	std::string p = proj;
	if (!p.empty() && p.back() != '/') p.push_back('/');
	p += "shaders";
	return normalize(p, true);
}

// Initialization API
void Path::SetLaunchDir(const std::string& dir) {
	g_launchDir = normalize(dir, true);
}

void Path::SetProjectDir(const std::string& dir) {
	g_projectDir = normalize(dir, true);
}

static std::string findProjectDirFromArgs(int argc, const char** argv) {
	if (!argv) return std::string();
	for (int i = 0; i < argc; ++i) {
		LOG("argv[" << i << "] = " << argv[i]);
		std::string a = argv[i];
		// forms: --projectdir=, --projectdir <value>, -ProjectDir=<value>, /ProjectDir=<value>
		auto eq = a.find('=');
		std::string key = (eq == std::string::npos) ? a : a.substr(0, eq);
		std::string val = (eq == std::string::npos) ? std::string() : a.substr(eq + 1);

		// make case-insensitive comparison for common flag forms
		std::string kl = toLowerCopy(key);
		if (kl == "--projectdir" || kl == "--project-dir" || kl == "-projectdir" || kl == "-project-dir" || kl == "/projectdir" || kl == "/project-dir") {
			if (!val.empty()) return val;
			if (i + 1 < argc) return std::string(argv[i + 1]);
		}
	}
	return std::string();
}

void Path::InitFromCmdLineAndEnv(int argc, const char** argv) {
	// launch dir: prefer exe current_path
	try { SetLaunchDir(std::filesystem::current_path().string()); } catch (...) { SetLaunchDir(std::string()); }

	// project dir: cmdline -> env -> fallback to launch
	std::string fromArgs = findProjectDirFromArgs(argc, argv);
	if (!fromArgs.empty()) { SetProjectDir(fromArgs); return; }

	const char* env = std::getenv("CARROTTOY_PROJECT_DIR");
	if (env && env[0]) { SetProjectDir(std::string(env)); return; }

	// fallback: treat launch dir as project dir
	SetProjectDir(LaunchDir());
}

} // namespace CarrotToy
