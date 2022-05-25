#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct IFigure {
    virtual const GLfloat* GetVertices() const = 0;

    virtual const GLfloat* GetColors() const = 0;

    virtual size_t SizeOf() const = 0;

    virtual size_t CntTriangles() const = 0;

    virtual ~IFigure() = default;
};
