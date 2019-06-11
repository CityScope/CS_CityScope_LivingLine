using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace OpenCVForUnityExample {
    public class PerspectiveToOrthogonal : MonoBehaviour
    {
        public GameObject orthPt0;
        public GameObject orthPt2;
        public GameObject orthPtA;
        public GameObject persPt0;
        public GameObject persPt1;
        public GameObject persPt2;
        public GameObject persPt3;
        public GameObject persPtA;

        public ArUcoWebCamTextureExample_RZ arUcoWebCamTextureExample_RZ;
        public List<Vector2> detectedTagPosRatio2D;
        public List<float> detectedTagRotDeg;

        private Vector3 pt0orth;
        private Vector3 pt2orth;
        private Vector3 ptAorth;
        private Vector3 pt0pers;
        private Vector3 pt1pers;
        private Vector3 pt2pers;
        private Vector3 pt3pers;
        private Vector3 ptApers;
        private Vector2 ptRatioA;

        void Start()
        {

        }

        void Update()
        {
            pt0orth = orthPt0.transform.position;
            pt2orth = orthPt2.transform.position;
            pt0pers = persPt0.transform.position;
            pt1pers = persPt1.transform.position;
            pt2pers = persPt2.transform.position;
            pt3pers = persPt3.transform.position;
            ptApers = persPtA.transform.position;

            // calculate x y ratio follow perspective principle
            ptRatioA = PointPerspectiveToOrthogonal(pt0pers, pt1pers, pt2pers, pt3pers, ptApers, false);
            //Debug.Log(string.Format("ptRatioA({0}, {1})", ptRatioA.x, ptRatioA.y));

            // calculate orthPtA
            ptAorth = PointRatioToOrthognal(pt0orth, pt2orth, ptRatioA);
            orthPtA.transform.position = ptAorth;

            // OpenCVForUnity WebCam ArUco tag position and rotation
            detectedTagPosRatio2D = arUcoWebCamTextureExample_RZ.detectedTagPosRatio2D;
            detectedTagRotDeg = arUcoWebCamTextureExample_RZ.detectedTagRotDeg;

            // draw all the tags in original/perspective or image and in orthogonal
            for (int i = 0; i < detectedTagPosRatio2D.Count; i ++)
            {
                // original/perspective
                Vector3 ptSpers = PointRatioToOrthognal(pt0orth, pt2orth, detectedTagPosRatio2D[i]);
                Vector3 ptEpers = PointRatioToOrthognal(pt0orth, pt2orth, detectedTagPosRatio2D[i] + new Vector2(0.05f, 0f));
                Vector3 ptRpers = PointRatioToOrthognal(pt0orth, pt2orth, detectedTagPosRatio2D[i] + Rotate(new Vector2(0.05f, 0f), detectedTagRotDeg[i]));
                Debug.DrawLine(ptSpers, ptEpers, Color.magenta);
                Debug.DrawLine(ptSpers, ptRpers, Color.magenta);
                // orthogonal
                // calculate x y ratio follow perspective principle
                Vector3 ptRatioS = PointPerspectiveToOrthogonal(pt0pers, pt1pers, pt2pers, pt3pers, ptSpers, false);
                Vector3 ptRatioE = PointPerspectiveToOrthogonal(pt0pers, pt1pers, pt2pers, pt3pers, ptEpers, false);
                Vector3 ptRatioR = PointPerspectiveToOrthogonal(pt0pers, pt1pers, pt2pers, pt3pers, ptRpers, false);
                // calculate orthPts
                Vector3 ptSorth = PointRatioToOrthognal(pt0orth, pt2orth, ptRatioS);
                Vector3 ptEorth = PointRatioToOrthognal(pt0orth, pt2orth, ptRatioE);
                Vector3 ptRorth = PointRatioToOrthognal(pt0orth, pt2orth, ptRatioR);
                Debug.DrawLine(ptSorth, ptEorth, Color.blue);
                Debug.DrawLine(ptSorth, ptRorth, Color.blue);
            }
        }

        Vector3 FindIntersection(Vector3 s1, Vector3 e1, Vector3 s2, Vector3 e2)
        {
            // https://rosettacode.org/wiki/Find_the_intersection_of_two_lines#C.23

            // Note: the intersection vector y value will be average of y value of 4 input vectors.

            float a1 = e1.z - s1.z;
            float b1 = s1.x - e1.x;
            float c1 = a1 * s1.x + b1 * s1.z;

            float a2 = e2.z - s2.z;
            float b2 = s2.x - e2.x;
            float c2 = a2 * s2.x + b2 * s2.z;

            float delta = a1 * b2 - a2 * b1;

            //If lines are parallel, the result will be (NaN, NaN).
            return delta == 0 ? new Vector3(float.NaN, float.NaN, float.NaN)
                : new Vector3((b2 * c1 - b1 * c2) / delta, (s1.y + e1.y + s2.y + e2.y) / 4.0f, (a1 * c2 - a2 * c1) / delta);
        }

        Vector2 PointPerspectiveToOrthogonal(Vector3 pt0, Vector3 pt1, Vector3 pt2, Vector3 pt3, Vector3 ptA, bool debugDrawLine = false)
        {

            // ref to the sketch!
            // follow perspective principle
            // returns normalized ratio of x y coordinates in orthogonal system

            // draw 4 edges
            if (debugDrawLine) Debug.DrawLine(pt0, pt1, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt1, pt2, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt2, pt3, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt3, pt0, Color.magenta);

            // define 2 intersections of 4 edges (2 vanishing points)
            // vanishing pt alpha
            Vector3 ptAlpha = FindIntersection(pt0, pt3, pt1, pt2);
            if (debugDrawLine) Debug.DrawLine(pt0, ptAlpha, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt1, ptAlpha, Color.magenta);
            // vanishing pt beta
            Vector3 ptBeta = FindIntersection(pt0, pt1, pt3, pt2);
            if (debugDrawLine) Debug.DrawLine(pt0, ptBeta, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt3, ptBeta, Color.magenta);
            // horizon line (line pt alpha beta)
            if (debugDrawLine) Debug.DrawLine(ptAlpha, ptBeta, Color.magenta);

            // define pt1' and pt3' with horizon line at pt0
            // find another pt on horizon line at pt0
            // ptH = pt0 + (ptBeta - ptAlpha)
            Vector3 ptH = pt0 + (ptBeta - ptAlpha);
            if (debugDrawLine) Debug.DrawLine(pt0, ptH, Color.white);
            // intersect horizon line at pt0 with 2 edge to find pt1' and pt3'
            Vector3 pt1a = FindIntersection(pt0, ptH, pt2, pt1);
            if (debugDrawLine) Debug.DrawLine(pt0, pt1a, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt1, pt1a, Color.magenta);
            Vector3 pt3a = FindIntersection(pt0, ptH, pt2, pt3);
            if (debugDrawLine) Debug.DrawLine(pt0, pt3a, Color.magenta);
            if (debugDrawLine) Debug.DrawLine(pt3, pt3a, Color.magenta);

            // define xOrth and yOrth by the ratio on horizon line
            // intersect horizon line at pt0 with line ptAlpha_ptA and line ptBeta_ptA
            Vector3 ptXa = FindIntersection(pt0, ptH, ptAlpha, ptA);
            if (debugDrawLine) Debug.DrawLine(ptAlpha, ptXa, Color.blue);
            Vector3 ptYa = FindIntersection(pt0, ptH, ptBeta, ptA);
            if (debugDrawLine) Debug.DrawLine(ptBeta, ptYa, Color.blue);
            // x y ratio
            float xRatio = (Vector3.Dot((ptXa - pt0), (pt1a - pt0)) / Mathf.Abs(Vector3.Dot((ptXa - pt0), (pt1a - pt0)))) * ((ptXa - pt0).magnitude / (pt1a - pt0).magnitude);
            float yRatio = (Vector3.Dot((ptYa - pt0), (pt3a - pt0)) / Mathf.Abs(Vector3.Dot((ptYa - pt0), (pt3a - pt0)))) * ((ptYa - pt0).magnitude / (pt3a - pt0).magnitude);

            return new Vector2(xRatio, yRatio);
        }

        Vector3 PointRatioToOrthognal(Vector3 pt0, Vector3 pt2, Vector2 ptRatioA)
        {
            // y is average of 4 input pts
            float y = (pt0.y + pt2.y) / 2.0f;

            // x, z are linear interperated
            float x = (pt2.x - pt0.x) * ptRatioA.x + pt0.x;
            float z = (pt2.z - pt0.z) * ptRatioA.y + pt0.z;

            return new Vector3(x, y, z);
        }

        Vector2 Rotate(Vector2 v, float degrees)
        {
            float sin = Mathf.Sin(degrees * Mathf.Deg2Rad);
            float cos = Mathf.Cos(degrees * Mathf.Deg2Rad);

            float tx = v.x;
            float ty = v.y;

            float vtx = (cos * tx) - (sin * ty);
            float vty = (sin * tx) + (cos * ty);
            Vector2 vt = new Vector2(vtx, vty);
            return vt;
        }
    }
}

