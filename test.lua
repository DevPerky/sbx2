---@module 'App-lib'
---@module 'Draw-lib'
require("math")

local window = nil

---@type Rectangle
local viewPort = { top = 0, left = 0, right = 800, bottom = 600 }

---@type integer
local projectionMatrix = 0

function OnWindowResize(width, height)
	viewPort.right = width
	viewPort.bottom = height
	MatrixSetOrtho(projectionMatrix, viewPort)
	DrawSetViewport(viewPort)
	DrawSetProjectionMatrix(projectionMatrix)
end

function Init()
	SetTargetFPS(90)
	window = CreateWindow(viewPort.right, viewPort.bottom, "test")
	projectionMatrix = MatrixNew()
	OnWindowResize(viewPort.right, viewPort.bottom)
	print(projectionMatrix)
end

function Update()
	DrawClear(0.1, 0.1, 0.1, 1.0)
	local seconds = GetSeconds()
	local x, y = GetMouseXY(window)
	local w, h = viewPort.right, viewPort.bottom
	
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