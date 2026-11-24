#include "mainview.h"

#include <math.h>

#include <QLoggingCategory>
#include <QOpenGLVersionFunctionsFactory>
#include <QApplication>

/**
 * @brief MainView::MainView
 * @param Parent
 */
MainView::MainView(QWidget* Parent) : QOpenGLWidget(Parent), scale(1.0f), virtualPos(0.0, 0.0), lastPos(0.0, 0.0) {}

/**
 * @brief MainView::~MainView Deconstructs the main view.
 */
MainView::~MainView() {
  debugLogger.stopLogging();
  makeCurrent();
}

/**
 * @brief MainView::initializeGL Initializes the opengl functions and settings,
 * initialises the renderers and sets up the debugger.
 */
void MainView::initializeGL() {
  initializeOpenGLFunctions();
  qDebug() << ":: OpenGL initialized";

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    QLoggingCategory::setFilterRules(
        "qt.*=false\n"
        "qt.text.font.*=false");
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    debugLogger.enableMessages();
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  makeCurrent();
  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  // grab the opengl context
  QOpenGLFunctions_4_1_Core* functions =
      QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(
          this->context());

  // initialize renderers here with the current context
  meshRenderer.init(functions, &settings);
}

/**
 * @brief MainView::resizeGL Handles window resizing.
 * @param newWidth The new width of the window in pixels.
 * @param newHeight The new height of the window in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  qDebug() << ".. resizeGL";

  settings.dispRatio = float(newWidth) / float(newHeight);

  settings.projectionMatrix.setToIdentity();
  settings.projectionMatrix.perspective(settings.FoV, settings.dispRatio, 0.1f,
                                        40.0f);
  updateMatrices();
}

/**
 * @brief MainView::updateMatrices Updates the matrices used for the model
 * transforms.
 */
void MainView::updateMatrices() {
  settings.modelViewMatrix.setToIdentity();
  settings.modelViewMatrix.translate(QVector3D(0.0, 0.0, -3.0));
  settings.modelViewMatrix.scale(scale);
  settings.modelViewMatrix.rotate(rotationQuaternion);

  settings.normalMatrix = settings.modelViewMatrix.normalMatrix();

  settings.uniformUpdateRequired = true;

  update();
}

/**
 * @brief MainView::updateBuffers Updates the buffers of the renderers.
 * @param mesh The mesh used to update the buffer content with.
 */
void MainView::updateBuffers(Mesh& mesh) {
  mesh.extractAttributes();
  meshRenderer.updateBuffers(mesh);
  update();
}

/**
 * @brief MainView::paintGL Draw call.
 */
void MainView::paintGL() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (settings.wireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (settings.modelLoaded) {
    meshRenderer.draw();
  }
}

/**
 * @brief MainView::toNormalizedScreenCoordinates Normalizes the mouse
 * coordinates to screen coordinates.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return A vector containing the normalized x and y screen coordinates.
 */
QVector2D MainView::toNormalizedScreenCoordinates(float x, float y) {
  float xRatio = x / float(width());
  float yRatio = y / float(height());

  // By default, the drawing canvas is the square [-1,1]^2:
  float xScene = (1 - xRatio) * -1 + xRatio * 1;
  // Note that the origin of the canvas is in the top left corner (not the lower
  // left).
  float yScene = yRatio * -1 + (1 - yRatio) * 1;

  return {xScene, yScene};
}

/**
 * @brief MainView::mouseMoveEvent Handles the dragging and rotating of the mesh
 * by looking at the mouse movement.
 * @param event Mouse event.
 */
void MainView::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() == Qt::LeftButton && dragging) {
    QVector2D sPos = toNormalizedScreenCoordinates(event->position().x(),
                                                   event->position().y());
    QVector3D newVec = QVector3D(sPos.x(), sPos.y(), 0.0);

    // project onto sphere
    float sqrZ = 1.0f - QVector3D::dotProduct(newVec, newVec);
    if (sqrZ > 0) {
      newVec.setZ(sqrt(sqrZ));
    } else {
      newVec.normalize();
    }

    QVector3D v2 = newVec.normalized();
    // reset if we are starting a drag
    if (!dragging) {
      dragging = true;
      oldVec = newVec;
      return;
    }

    // calculate axis and angle
    QVector3D v1 = oldVec.normalized();
    QVector3D N = QVector3D::crossProduct(v1, v2).normalized();
    if (N.length() == 0.0f) {
      oldVec = newVec;
      return;
    }
    float dot = QVector3D::dotProduct(v1, v2);
    dot = std::max(-1.0f, std::min(1.0f, dot)); // clamp
    float angle = 180.0f / M_PI * acos(dot);

    rotationQuaternion =
        QQuaternion::fromAxisAndAngle(N, angle) * rotationQuaternion;
    updateMatrices();

    // for next iteration
    oldVec = newVec;
  } else {
    // to reset drag
    oldVec = QVector3D();
  }
}

void MainView::rotateArcballFromVirtualPos()
{
    QVector2D sPos = virtualPos;   // already normalized-ish

    QVector3D newVec(sPos.x(), sPos.y(), 0.0);

    // project onto unit sphere
    float sqrZ = 1.0f - QVector3D::dotProduct(newVec, newVec);
    if (sqrZ > 0) {
        newVec.setZ(std::sqrt(sqrZ));
    } else {
        newVec.normalize();
    }

    QVector3D v2 = newVec.normalized();

    if (!hasOldVec) {
        oldVec = newVec;
        hasOldVec = true;
        return;
    }

    QVector3D v1 = oldVec.normalized();
    QVector3D N = QVector3D::crossProduct(v1, v2);

    if (N.lengthSquared() < 1e-6f)
        return;

    N.normalize();
    float dot = QVector3D::dotProduct(v1, v2);
    dot = qBound(-1.0f, dot, 1.0f);
    float angle = qRadiansToDegrees(std::acos(dot));

    rotationQuaternion = QQuaternion::fromAxisAndAngle(N, angle) * rotationQuaternion;
    updateMatrices();

    oldVec = newVec;
}

/**
 * @brief MainView::mousePressEvent Handles presses by the mouse. Currently does
 * nothing except setting focus.
 * @param event Mouse event.
 */
void MainView::mousePressEvent(QMouseEvent* event) {
    setFocus();

    if (event->buttons() == Qt::LeftButton) {

        dragging = true;
        oldVec = QVector3D();

        hasOldVec = false;
        virtualPos = QVector2D(0, 0);
        lastPos = event->globalPosition();
        setCursor(Qt::BlankCursor);

        // Start receiving ALL mouse releases globally
        qApp->installEventFilter(this);
    }
}


/**
 * @brief MainView::mousePressEvent Handles presses by the mouse. Currently does
 * nothing except setting focus.
 * @param event Mouse event.
 */
void MainView::mouseReleaseEvent(QMouseEvent* event) {
    setFocus();
    if (event->buttons() == Qt::LeftButton) {
        dragging = false;
        unsetCursor();
        releaseMouse();
    }
}

void MainView::focusOutEvent(QFocusEvent* event) {
    if (dragging) {
        releaseMouse();
        unsetCursor();
        dragging = false;
    }
    QWidget::focusOutEvent(event);
}

bool MainView::eventFilter(QObject *obj, QEvent *event)
{
    if (dragging &&
        (event->type() == QEvent::MouseButtonRelease ||
         event->type() == QEvent::MouseButtonDblClick))
    {
        dragging = false;

        QCursor::setPos(lastPos.toPoint());

        unsetCursor();
        qApp->removeEventFilter(this);

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

/**
 * @brief MainView::wheelEvent Handles zooming of the view.
 * @param event Mouse event.
 */
void MainView::wheelEvent(QWheelEvent* event) {
  // Delta is usually 120
  float phi = 1.0f + (event->angleDelta().y() / 2000.0f);
  scale = fmin(fmax(phi * scale, 0.01f), 100.0f);
  updateMatrices();
}

/**
 * @brief MainView::keyPressEvent Handles keyboard shortcuts. Currently support
 * 'Z' for wireframe mode and 'R' to reset orientation.
 * @param event Mouse event.
 */
void MainView::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case 'Z':
      settings.wireframeMode = !settings.wireframeMode;
      update();
      break;
    case 'R':
      scale = 1.0f;
      rotationQuaternion = QQuaternion();
      updateMatrices();
      update();
      break;
  }
}

/**
 * @brief MainView::onMessageLogged Helper function for logging messages.
 * @param message The message to log.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
