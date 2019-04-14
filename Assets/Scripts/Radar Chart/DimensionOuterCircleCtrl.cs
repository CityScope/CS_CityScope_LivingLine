using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DimensionOuterCircleCtrl : MonoBehaviour
{
    public CityMatrixRadarChart CMRadarChart;
    public GameObject[] density;
    public GameObject[] densityProfile;
    public GameObject[] diversity;
    public GameObject[] diversityProfile;
    public GameObject[] interaction;
    public GameObject[] interactionProfile;

    void Update()
    {
        float d0 = CMRadarChart.dimensionMetrics[0];
        float d1 = CMRadarChart.dimensionMetrics[1];
        float d2 = CMRadarChart.dimensionMetrics[2];

        for (int i = 0; i < density.Length; i++)
        {
            float ab = 1.0001f / density.Length;
            float a = i * ab;
            float b = (i + 1) * ab;
            if (d0 >= a && d0 < b)
            {
                density[i].SetActive(true);
                densityProfile[i].SetActive(true);
            }
            else
            {
                density[i].SetActive(false);
                densityProfile[i].SetActive(false);
            }
            if (d1 >= a && d1 < b)
            {
                diversity[i].SetActive(true);
                diversityProfile[i].SetActive(true);
            }
            else
            {
                diversity[i].SetActive(false);
                diversityProfile[i].SetActive(false);
            }
            if (d2 >= a && d2 < b)
            {
                interaction[i].SetActive(true);
                interactionProfile[i].SetActive(true);
            }
            else
            {
                interaction[i].SetActive(false);
                interactionProfile[i].SetActive(false);
            }
        }

    }
}
