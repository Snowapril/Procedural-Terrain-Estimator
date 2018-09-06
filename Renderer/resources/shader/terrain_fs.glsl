#version 430 core

out vec4 fragColor;
flat in uint tes_tessLevel;

void main(void)
{
	vec4 color;

	if (tes_tessLevel == 1)
		color = vec4(0.0, 0.0, 1.0, 1.0);
	else if (tes_tessLevel == 2)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	else if (tes_tessLevel == 3)
		color = vec4(1.0, 1.0, 0.0, 1.0);
	else if (tes_tessLevel == 4)
		color = vec4(1.0, 0.0, 1.0, 1.0);
	else if (tes_tessLevel == 5)
		color = vec4(0.75, 0.75, 0.75, 1.0);
	else
		color = vec4(0.0, 1.0, 1.0, 1.0);

	fragColor = color;
}