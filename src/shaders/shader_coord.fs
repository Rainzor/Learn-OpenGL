#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{   
    //texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标。
    //texture函数会使用之前设置的纹理参数对相应的颜色值进行采样。
    //这个片段着色器的输出就是纹理的（插值）纹理坐标上的(过滤后的)颜色。

    // mix函数是一个线性插值函数，它的第一个参数是起始值，第二个参数是终止值，第三个参数是插值因子。
    FragColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);
}