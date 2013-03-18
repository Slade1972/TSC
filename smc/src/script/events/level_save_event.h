// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4 -*-
#ifndef SMC_SCRIPT_LEVEL_SAVE_EVENT_H
#define SMC_SCRIPT_LEVEL_SAVE_EVENT_H
#include <string>
#include "../script.h"
#include "../../user/savegame.h"
#include "event.h"
namespace SMC{
	namespace Script{
		class cLevel_Save_Event: public cEvent
		{
		public:
			cLevel_Save_Event(Lua_Save_Data* p_data);
			Lua_Save_Data* Get_Save_Data();
			virtual std::string Event_Name();
			virtual int Run_Lua_Callback(cLua_Interpreter* p_lua);
		private:
			Lua_Save_Data* mp_data;
		};
	};
};
#endif