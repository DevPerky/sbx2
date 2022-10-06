---@module 'App-lib'
---@module 'Draw-lib'
require("math")

local window = nil

---@type Camera
local camera = {
	position = { x=0, y=0},
	viewPort = { top = 0, left = 0, right = 800, bottom = 600 }
}

---@type integer
local projectionMatrix = 0

function OnWindowResize(width, height)
	camera.viewPort.right = width
	camera.viewPort.bottom = height
	MatrixSetOrtho(projectionMatrix, camera.viewPort)
	DrawSetViewport(camera.viewPort)
	DrawSetProjectionMatrix(projectionMatrix)
end

function Init()
	SetTargetFPS(90)
	window = CreateWindow(camera.viewPort.right, camera.viewPort.bottom, "test")
	projectionMatrix = MatrixNew()
	OnWindowResize(camera.viewPort.right, camera.viewPort.bottom)
	print(projectionMatrix)
end

function Update()
	DrawClear(0.1, 0.1, 0.1, 1.0)
	local seconds = GetSeconds()
	local x, y = GetMouseXY(window)
	local w, h = camera.viewPort.right, camera.viewPort.bottom
	
	---@type Vector2
	local mousePos = { x=x, y=y }

	DrawSetColor(0, 0, 1, 1)
	DrawLine(0, 0, x, y, 10);
	DrawSetColor(0, 1, 0, 1);
	DrawRectangle(x - 5, y - 5, x + 5, y + 5)
	DrawCircle(mousePos, (math.cos(seconds * 50)/2 + 1) * 20)
	DrawSetColor(1, 0, 0, 1);
	DrawRectangle(0, h-20, (seconds/60)*w, h)
end