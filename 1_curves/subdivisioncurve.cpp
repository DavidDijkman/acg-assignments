#include "subdivisioncurve.h"

#include <QDebug>
#include <QTextStream>

/**
 * @brief SubdivisionCurve::SubdivisionCurve Creates a new subdivision curve.
 */
SubdivisionCurve::SubdivisionCurve() {}

/**
 * @brief SubdivisionCurve::presetNet Constructs a control net preset based on a
 * dropdown index.
 * @param preset Index of the preset.
 */
void SubdivisionCurve::presetNet(int preset) {
  netCoords.clear();

  switch (preset) {
    case 0:
      // Pentagon
      netCoords.reserve(5);
      netCoords.append(QVector2D(-0.25f, -0.5f));
      netCoords.append(QVector2D(-0.75f, 0.0f));
      netCoords.append(QVector2D(-0.25f, 0.75f));
      netCoords.append(QVector2D(0.75f, 0.5f));
      netCoords.append(QVector2D(0.5f, -0.75f));
      break;
    case 1:
      // Basis
      netCoords.reserve(9);
      netCoords.append(QVector2D(-1.0f, -0.25f));
      netCoords.append(QVector2D(-0.75f, -0.25f));
      netCoords.append(QVector2D(-0.5f, -0.25f));
      netCoords.append(QVector2D(-0.25f, -0.25f));
      netCoords.append(QVector2D(0.0f, 0.50f));
      netCoords.append(QVector2D(0.25f, -0.25f));
      netCoords.append(QVector2D(0.5f, -0.25f));
      netCoords.append(QVector2D(0.75f, -0.25f));
      netCoords.append(QVector2D(1.0f, -0.25f));
      break;
    case 2:
      // G
      netCoords.reserve(14);
      netCoords.append(QVector2D(0.75f, 0.35f));
      netCoords.append(QVector2D(0.75f, 0.75f));
      netCoords.append(QVector2D(-0.75f, 0.75f));
      netCoords.append(QVector2D(-0.75f, -0.75f));
      netCoords.append(QVector2D(0.75f, -0.75f));
      netCoords.append(QVector2D(0.75f, 0.0f));
      netCoords.append(QVector2D(0.0f, 0.0f));
      netCoords.append(QVector2D(0.0f, -0.2f));
      netCoords.append(QVector2D(0.55f, -0.2f));
      netCoords.append(QVector2D(0.55f, -0.55f));
      netCoords.append(QVector2D(-0.55f, -0.55f));
      netCoords.append(QVector2D(-0.55f, 0.55f));
      netCoords.append(QVector2D(0.55f, 0.55f));
      netCoords.append(QVector2D(0.55f, 0.35f));
      break;
  }
}

// TODO: create some sort of subdivide function

/**
 * @brief SubdivisionCurve::addPoint Adds a point to the control net.
 * @param p The point to add to the control net.
 */
void SubdivisionCurve::addPoint(QVector2D p) {
  netCoords.append(p);
  // you should probably recalculate the curve
}

/**
 * @brief SubdivisionCurve::setPointPosition Changes the position of a point in
 * the control net.
 * @param idx The index of the point to update the position of.
 * @param p The new position of the point.
 */
void SubdivisionCurve::setPointPosition(int idx, QVector2D p) {
  netCoords[idx] = p;
  // you should probably recalculate the curve
}

/**
 * @brief SubdivisionCurve::removePoint Removes a point from the control net.
 * @param idx The index of the point to remove.
 */
void SubdivisionCurve::removePoint(int idx) {
  netCoords.remove(idx);
  // you should probably recalculate the curve
}

/**
 * @brief SubdivisionCurve::setMask Set the first and second stencil masks based
 * on the provided string. Note that first/second stencil does not necessarily
 * correspond to odd/even stencil; it might be even/odd!
 * @param stringMask The string mask containing integers.
 */
void SubdivisionCurve::setMask(const QString& stringMask) {
  subdivMask.clear();

  QString trimmedMask = stringMask.trimmed();

  // Convert to sequence of integers
  QTextStream intSeq(&trimmedMask);
  while (!intSeq.atEnd()) {
    int k;
    intSeq >> k;
    subdivMask.append(k);
  }

  firstStencil.clear();
  secondStencil.clear();

  for (int k = 0; k < subdivMask.size(); ++k) {
    if (k % 2) {
      firstStencil.append(subdivMask[k]);
    } else {
      secondStencil.append(subdivMask[k]);
    }
  }

  // Note that first/second is not always odd/even; it might be even/odd
  qDebug() << ":: Extracted stencils" << firstStencil << "and" << secondStencil;
}

/**
 * @brief SubdivisionCurve::findClosest Finds the index of the closest point in
 * the control net to the provided point.
 * @param p The point to find the closest point to.
 * @param maxDist The maximum distance a point and the provided point can have.
 * Is a value between 0 and 1.
 * @return The index of the closest point to the provided point. Returns -1 if
 * no point was found within the maximum distance.
 */
int SubdivisionCurve::findClosest(const QVector2D& p, const float maxDist) {
  int ptIndex = -1;
  float currentDist, minDist = 4;

  for (int k = 0; k < netCoords.size(); k++) {
    currentDist = netCoords[k].distanceToPoint(p);
    if (currentDist < minDist) {
      minDist = currentDist;
      ptIndex = k;
    }
  }
  if (minDist >= maxDist) {
    return -1;
  }

  return ptIndex;
}
