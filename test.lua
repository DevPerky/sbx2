window = nil

function init()
	window = CreateWindow(800, 600, "test")
	SetTargetFPS(90)
end

function update()
	Clear(0.1, 0.1, 0.1, 1.0)
	seconds = GetSeconds()
	x, y = GetMouseXY(window)
	w, h = GetWindowDimensions(window);
	
	SetDrawColor(0, 0, 1, 1)
	DrawLine(0, 0, x, y, 10);
	SetDrawColor(1, 0, 0, 1);
	DrawRectangle(0, h-20, (seconds/60)*w, h)
	SetDrawColor(0, 1, 0, 1);
	DrawRectangle(x - 5, y - 5, x + 5, y + 5)
end