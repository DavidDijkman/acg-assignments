#include "curverenderer.h"

/**
 * @brief CurveRenderer::~CurveNetRenderer Destructor of the subdivision curve
 * renderer.
 */
CurveRenderer::~CurveRenderer() {
  gl->glDeleteVertexArrays(1, &vao);
  gl->glDeleteBuffers(1, &vbo);
}

/**
 * @brief CurveRenderer::initShaders Initialises the different shaders used
 * in the subdivision curve renderer.
 */
void CurveRenderer::initShaders() {
  shaders.insert(ShaderType::FLAT, constructDefaultShader("flat"));

  // TODO: add your geometry shader(s) here
  // You can use a similar approach to the constructDefaultShader method
}

/**
 * @brief CurveRenderer::initBuffers Initialises the buffers.
 */
void CurveRenderer::initBuffers() {
  // TODO
}

/**
 * @brief CurveRenderer::updateBuffers Updates the contents of the buffers.
 * @param sc The subdivision curve containing the information to update the
 * buffer(s) with.
 */
void CurveRenderer::updateBuffers(SubdivisionCurve& curve) {
  // TODO:
}

/**
 * @brief CurveRenderer::draw Draws the subdivision curve on the screen.
 * Uses the contents of the vertex array buffer for drawing. The size of the
 * draw array is expected to be stored in vboSize.
 */
void CurveRenderer::draw() {
  shaders[settings->currentShader]->bind();

  gl->glBindVertexArray(vao);

  // TODO: draw something

  gl->glBindVertexArray(0);

  shaders[settings->currentShader]->release();
}
