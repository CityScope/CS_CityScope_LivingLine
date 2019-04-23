using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BarChart : MonoBehaviour
{

    [Range(0, 2)]
    public int vizType = 0;
    [Range(0, 1f)]
    public float[] metrics;

    public int numH = 6;
    public float spacingH = 20.0f;

    public GameObject barsRoot;
    public GameObject barSource;
    public string barGoName = "bar profile 0";
    public List<GameObject> bars;
    public List<MeshRenderer> meshRenderers;
    public Material[] vizFillMaterials;
    public Material[] vizProfileMaterials;


    void Start()
    {
        // clone all bars profiles horizontally
        for (int i = 0; i < numH; i++)
        {
            GameObject go = Instantiate(barSource, barsRoot.transform);
            go.transform.localPosition = new Vector3(i * spacingH, 0.0f, 0.0f);
            bars.Add(go);
            foreach (MeshRenderer mr in barsRoot.transform.GetComponentsInChildren<MeshRenderer>())
            {
                meshRenderers.Add(mr);
            }
        }

        InvokeRepeating("UpdateBarChart", 1, 1f);
    }


    void UpdateBarChart()
    {
        // enable or disable gos by metrics
        for (int j=0;j<bars.Count;j++)
        {
            GameObject bar = bars[j];
            for (int i = 0; i < bar.transform.childCount; i ++)
            {
                if (metrics[j] > 1.0f / bar.transform.childCount * i && metrics[j] <= 1.0f / bar.transform.childCount * (i + 1))
                {
                    bar.transform.Find(string.Format("bar profile 0{0}", i + 1)).gameObject.SetActive(true);
                }
                else
                {
                    bar.transform.Find(string.Format("bar profile 0{0}", i + 1)).gameObject.SetActive(false);
                }
            }
        }

        
        // decide color by vizType
        foreach (MeshRenderer mr in meshRenderers)
        {
            if (mr.sharedMaterial.name.Contains("Line"))
            {
                if(mr.sharedMaterial != vizProfileMaterials[vizType])
                     mr.sharedMaterial = vizProfileMaterials[vizType];
            }
            else
            {
                if (mr.sharedMaterial != vizFillMaterials[vizType])
                    mr.sharedMaterial = vizFillMaterials[vizType];
            }
        }

    }

}
