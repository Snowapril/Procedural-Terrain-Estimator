#version 430 core

in float tes_tessLevel;

out vec4 fragColor;

void main(void)
{
	vec4 color;

	if (tes_tessLevel == 1.0)
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (tes_tessLevel == 2.0)
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (tes_tessLevel == 3.0)
		color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (tes_tessLevel == 4.0)
		color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (tes_tessLevel == 5.0)
		color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	else
		color = vec4(1.0f, 0.0f, 1.0f, 1.0f);

	fragColor = color;
}