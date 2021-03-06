#pragma once

#include "debugger.h"
#include "dbg_custom.h"
#include "dbg_redirect.h"
#include "dbg_protocol.h"  
#include "dbg_io.h"
#include <functional>	
#include <vector>	
#include <net/tcp/buffer.h>

struct lua_State;

class launch_io
	: public vscode::io
{
public:
	enum class encoding {
		none,
		ansi,
		utf8,
	};

public:
	launch_io(const std::string& console);
	void              update(int ms);
	bool              output(const vscode::wprotocol& wp);
	vscode::rprotocol input();
	bool              input_empty() const;
	void              close();
	void              send(vscode::rprotocol&& rp);
	bool          	  enable_console() const;

private:
	bool update_();
	void output_(const char* str, size_t len);

private:
	std::vector<char>                      buffer_;
	net::tcp::buffer<vscode::rprotocol, 8> input_queue_;
	encoding                               encoding_;
};

class launch_server
	: public vscode::custom
{
public:
	launch_server(const std::string& console, std::function<void()> idle);
	void update();
	void send(vscode::rprotocol&& rp);

private:
	virtual void set_state(vscode::state state);
	virtual void update_stop();
	void         update_redirect();
	static int   print(lua_State *L);
	static int   print_empty(lua_State *L);

private:
	vscode::debugger debugger_;
	vscode::state state_;
	vscode::redirector stderr_;
	std::function<void()> idle_;
	launch_io io_;
};

