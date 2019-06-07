using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PerspectiveToOrthogonal : MonoBehaviour
{
    public GameObject orthPt0;
    public GameObject orthPt1;
    public GameObject orthPt2;
    public GameObject orthPt3;
    public GameObject orthPtA;
    public GameObject persPt0;
    public GameObject persPt1;
    public GameObject persPt2;
    public GameObject persPt3;
    public GameObject persPtA;
    
    void Start()
    {
        orthPtA = transform.gameObject;
    }
    
    void Update()
    {
        // draw 4 edges
        Debug.DrawLine(persPt0.transform.position, persPt1.transform.position, Color.magenta);
        Debug.DrawLine(persPt1.transform.position, persPt2.transform.position, Color.magenta);
        Debug.DrawLine(persPt2.transform.position, persPt3.transform.position, Color.magenta);
        Debug.DrawLine(persPt3.transform.position, persPt0.transform.position, Color.magenta);

        // draw 2 intersections of 4 edges
        Vector3 pt0132 = FindIntersection(persPt0.transform.position, persPt1.transform.position, persPt3.transform.position, persPt2.transform.position);
        Debug.DrawLine(persPt0.transform.position, pt0132, Color.magenta);
        Debug.DrawLine(persPt3.transform.position, pt0132, Color.magenta);
        Vector3 pt0312 = FindIntersection(persPt0.transform.position, persPt3.transform.position, persPt1.transform.position, persPt2.transform.position);
        Debug.DrawLine(persPt0.transform.position, pt0312, Color.magenta);
        Debug.DrawLine(persPt1.transform.position, pt0312, Color.magenta);

        // 
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
            : new Vector3((b2 * c1 - b1 * c2) / delta, 0.0f, (a1 * c2 - a2 * c1) / delta);
    }
}
