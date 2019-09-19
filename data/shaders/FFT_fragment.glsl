#version 330 core

precision highp float;

//-------------------------------------------------------------------------------------------------
// Uniforms
uniform sampler2D u_source;
uniform int u_size;
uniform int u_iteration;
uniform int u_horiz;

//-------------------------------------------------------------------------------------------------
// Interpolated
smooth in vec2 v_uv;

//-------------------------------------------------------------------------------------------------
// Constants
const float pi = 3.14159265358;

//-------------------------------------------------------------------------------------------------
// Out
out vec4 out_Color;

//-------------------------------------------------------------------------------------------------
vec2 multiplyComplex(vec2 a, vec2 b)
{
	return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

//-------------------------------------------------------------------------------------------------
vec2 W(float N, float i)
{	
	float arg = -2*pi*i/N;
	return vec2(cos(arg), sin(arg));
}

//-------------------------------------------------------------------------------------------------
void main()
{
	// Stockham

	// i=0 - we have N FFTs with size 1
	// i=1 - we have N/2 FFTs with size 2

	// u = n div N/pow(2, i)
	// Out[n] = In[n + u*N/pow(2, i)] + W*In[n + u*N/pow(2, i) + N/pow(2, i)]

    float N = u_size;
	float i = u_iteration;

	float n;

	if (u_horiz != 0)
	{
		n = floor(v_uv.x * N);
	}
	else
	{
		n = floor(v_uv.y * N);
	}

	// current subtransform size
	float num = N/pow(2, i);
	float u = floor(n/num);

	float evenIndex = mod(n + u*num, N);
	float oddIndex = mod(n + u*num + num, N);

	vec4 even;
	vec4 odd;

	if (u_horiz != 0)
	{
		even = texture2D(u_source, vec2(evenIndex/N, v_uv.y)).rgba;
		odd = texture2D(u_source, vec2(oddIndex/N, v_uv.y)).rgba;
	}
	else
	{
		even = texture2D(u_source, vec2(v_uv.x, evenIndex/N)).rgba;
		odd = texture2D(u_source, vec2(v_uv.x, oddIndex/N)).rgba;
	}

	vec2 Wn = W(pow(2, i), -u);

	out_Color = even + vec4(multiplyComplex(Wn, odd.rg), multiplyComplex(Wn, odd.ba));
}
