#version 460 core

uniform uint meshID = 1;

out uint FragColor;

void main(){
    FragColor = meshID;
}