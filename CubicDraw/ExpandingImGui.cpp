#include "ExpandingImGui.h"

ExpandingImGui::ExpandingImGui()
{
	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Fonts\\ARIALUNI.TTF", this->fontSize, 
		NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
}

void ExpandingImGui::FontCenterAlignment(UINT str_len)
{
	this->lastFontX = centerX - (str_len * this->fontSize * 0.5f) / 2;
	this->lastFontY = centerY - this->fontSize - 50.0f;
	ImGui::SetCursorPos({ this->lastFontX, this->lastFontY });
}

void ExpandingImGui::TriggerCenterImGui(std::string contents, UINT isTrigger)
{
	// window emerge
	if (ImGui::Begin("", 0, 3))
	{
		if (isTrigger)
		{
			this->presSizeX = (this->presSizeX + this->stepX) >= this->strecthedWidth ?
				this->strecthedWidth : (this->presSizeX + this->stepX);
			this->presSizeY = (this->presSizeY + this->stepY) >= this->strecthedHeight ?
				this->strecthedHeight : (this->presSizeY + this->stepY);
			if (this->presSizeX + this->stepX >= this->strecthedWidth &&
				this->presSizeY + this->stepY >= this->strecthedHeight)
			{
				this->FontCenterAlignment(contents.length());
				ImGui::TextWrapped(contents.c_str());
			}
			ImGui::SetWindowSize({ this->presSizeX , this->presSizeY });
			ImGui::SetWindowPos({ this->centerX - this->presSizeX / 2, this->centerY - this->presSizeY / 2 });
		}
		else
		{
			ImGui::SetWindowSize({ 0 , 0 });
			ImGui::SetWindowPos({ -200, -200 });
		}
		ImGui::End();
	}
}

void ExpandingImGui::TriggerImGui(std::string contents, UINT isTrigger)
{
	// window emerge
	if (ImGui::Begin("", 0, 3))
	{
		if (isTrigger)
		{
			this->presSizeX = (this->presSizeX + this->stepX) >= this->strecthedWidth ?
				this->strecthedWidth : (this->presSizeX + this->stepX);
			this->presSizeY = (this->presSizeY + this->stepY) >= this->strecthedHeight ?
				this->strecthedHeight : (this->presSizeY + this->stepY);
			if (this->presSizeX + this->stepX >= this->strecthedWidth &&
				this->presSizeY + this->stepY >= this->strecthedHeight)
			{
				ImGui::SetCursorPos({ 100, 100 });
				ImGui::PushTextWrapPos(this->strecthedWidth - 100);
				ImGui::TextWrapped(contents.c_str());
			}
			ImGui::SetWindowSize({ this->presSizeX , this->presSizeY });
			ImGui::SetWindowPos({ this->centerX - this->presSizeX / 2, this->centerY - this->presSizeY / 2 });
		}
		else
		{
			ImGui::SetWindowSize({ 0 , 0 });
			ImGui::SetWindowPos({ -200, -200 });
		}
		ImGui::End();
	}
}