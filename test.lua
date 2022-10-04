---@module 'App-lib'
---@module 'Draw-lib'

local window = nil

---@type Camera
local camera = {
	position = { x=0, y=0},
	viewPort = { top = 0, left = 0, right = 800, bottom = 600 }
}

function OnWindowResize(width, height)
	camera.viewPort.right = width
	camera.viewPort.bottom = height
	SetDrawCamera(camera)
end

function Init()
	window = CreateWindow(camera.viewPort.right, camera.viewPort.bottom, "test")
	SetDrawCamera(camera)
	SetTargetFPS(90)
end

function Update()
	Clear(0.1, 0.1, 0.1, 1.0)
	local seconds = GetSeconds()
	local x, y = GetMouseXY(window)
	local w, h = camera.viewPort.right, camera.viewPort.bottom
	
	SetDrawColor(0, 0, 1, 1)
	DrawLine(0, 0, x, y, 10);
	SetDrawColor(1, 0, 0, 1);
	DrawRectangle(0, h-20, (seconds/60)*w, h)
	SetDrawColor(0, 1, 0, 1);
	DrawRectangle(x - 5, y - 5, x + 5, y + 5)
end