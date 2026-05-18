#version 460 core

uniform uint meshID;

out uint FragColor;

void main(){
    FragColor = meshID;
}