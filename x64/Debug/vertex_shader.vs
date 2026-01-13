#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model; // এটি দিয়ে পজিশন, রোটেশন ও স্কেল সব একসাথে কন্ট্রোল হবে

void main() {
    gl_Position = model * vec4(aPos, 1.0);
}