---@module 'App-lib'
---@module 'Draw-lib'

local window = nil

function Init()
	window = CreateWindow(800, 600, "test")
	SetTargetFPS(90)
end

function Update()
	Clear(0.1, 0.1, 0.1, 1.0)
	local seconds = GetSeconds()
	local x, y = GetMouseXY(window)
	local w, h = GetWindowDimensions(window);
	
	SetDrawColor(0, 0, 1, 1)
	DrawLine(0, 0, x, y, 10);
	SetDrawColor(1, 0, 0, 1);
	DrawRectangle(0, h-20, (seconds/60)*w, h)
	SetDrawColor(0, 1, 0, 1);
	DrawRectangle(x - 5, y - 5, x + 5, y + 5)
end