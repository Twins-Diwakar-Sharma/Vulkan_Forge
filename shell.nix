{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {

  packages = with pkgs; [
    cmake
    gcc
    glfw
    wayland

    vulkan-loader
    vulkan-headers
    vulkan-validation-layers
    vulkan-tools
    shaderc  #provides glslc
    #spriv-tools
    #spriv-headers
  ];

  inputsFrom = with pkgs; [
    glfw
    wayland
  ];

  LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath [pkgs.wayland pkgs.glfw pkgs.vulkan-loader pkgs.vulkan-headers pkgs.vulkan-validation-layers pkgs.vulkan-tools]}";
}
