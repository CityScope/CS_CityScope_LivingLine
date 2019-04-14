using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class CityMatrixRadarChart : MonoBehaviour {

    // shared
    public GameObject centerRefPt;
    public GameObject[] endRefPts;

    // current
    public GameObject[] movingRefPts;
    [Range(0, 1f)]
    public float[] metrics;
    public GameObject currentLineAll;

    //private float[] prev = new float[9];
    private Vector3[] movingPts = new Vector3[9];

    private void Update()
    {
    
        // CURRENT
        // update moving ref pts' locations
        for (int i = 0; i < metrics.Length; i++)
        {
            Vector3 start = centerRefPt.transform.position;
            Vector3 end = endRefPts[i].transform.position;
            Vector3 v = (end - start) * metrics[i];
            Vector3 p = start + v;
            movingPts[i] = p;
            // moving ref pt gameobjects for fill mesh
            movingRefPts[i].transform.position = p;
        }

        // update lines
        currentLineAll.GetComponent<LineRenderer>().SetPosition(0, movingPts[0]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(1, movingPts[1]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(2, movingPts[2]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(3, movingPts[3]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(4, movingPts[4]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(5, movingPts[5]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(6, movingPts[6]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(7, movingPts[7]);
        currentLineAll.GetComponent<LineRenderer>().SetPosition(8, movingPts[8]);
    }
            

}
