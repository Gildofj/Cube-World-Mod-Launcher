#pragma once
#include <string>
#include "cwsdk.h"
#include "../ModWidget.h"

plasma::Node* CreateModWidgetGUI(cube::Game* game)
{
    if (!game || !game->plasma_engine || !game->plasma_engine->root_node) {
        CW_LOG_WARN("Cannot CreateModWidgetGUI: game or plasma_engine root_node is NULL.");
        return nullptr;
    }
    if (!game->gui.blackwidget_node_0) {
        CW_LOG_WARN("Cannot CreateModWidgetGUI: blackwidget_node_0 is NULL.");
        return nullptr;
    }

	// Variables
	FloatVector2 size((float)game->width, (float)game->height);
	std::wstring wstr_node_name(L"mod-node");

	// Create node to add to the engine root node (automatically gets drawn)
	plasma::Node* node = game->plasma_engine->CreateNode(game->plasma_engine->root_node, &wstr_node_name);
	if (!node) {
        CW_LOG_WARN("Cannot CreateModWidgetGUI: CreateNode returned NULL.");
        return nullptr;
    }
	
	// Create a deep copy of the blackwidget (background node)
	plasma::Node* background = game->gui.blackwidget_node_0->CreateDeepCopy(node);
	if (background && background->widget1) {
        background->widget1->SetSize(&size);
        background->Translate(0, 0, 0, 0);
    }

	mod::ModWidget::Init();
	mod::ModWidget* widget = (mod::ModWidget*)new char[sizeof(mod::ModWidget)];

	// Create a widget and add it to the node specified.
	widget->ctor(game, node, background, &allDlls);

	node->SetVisibility(false);

	return node;
}

extern "C" void GameTickHandler(cube::Game* game) {
    cube::Game* real_game = cube::GetGame();
    if (!real_game) real_game = game;
    if (!real_game) return;

	static bool init = false;
	if (!init)
	{
        init = true;
        CW_LOG_INFO("GameTickHandler: First game tick active. Game engine running stably.");
	}

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGameTickPriority == (GenericMod::Priority)priority) {
				dll->mod->OnGameTick(real_game);
			}
		}
	}
}

GETTER_VAR(void*, ASMGameTickHandler_jmpback);
extern "C" void ASMGameTickHandler();

void SetupGameTickHandler() {
    WriteFarJMP(Offset(base, 0x136458), (void*)&ASMGameTickHandler);
	ASMGameTickHandler_jmpback = Offset(base, 0x13646B);
}
