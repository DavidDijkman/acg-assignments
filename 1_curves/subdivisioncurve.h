#ifndef SUBDIVISIONCURVE_H
#define SUBDIVISIONCURVE_H

#include <QString>
#include <QVector2D>
#include <QVector>

/**
 * @brief The SubdivisionCurve class contains the data of a 2D subdivision
 * curve.
 */
class SubdivisionCurve {
 public:
  SubdivisionCurve();

  inline QVector<QVector2D> getNetCoords() { return netCoords; }

  void setMask(const QString& stringMask);
  void presetNet(int preset);

  int findClosest(const QVector2D& p, const float maxDist);

  void addPoint(QVector2D p);
  void setPointPosition(int idx, QVector2D p);
  void removePoint(int idx);

 private:
  QVector<int> subdivMask, firstStencil, secondStencil;

  QVector<QVector2D> curveCoords;

  QVector<QVector2D> netCoords;
};

#endif  // SUBDIVISIONCURVE_H
