using UnityEngine;
using System.Collections;

public class Heatmap : MonoBehaviour
{

    public float xPosMin;
    public float xPosMax;
    public float yPosMin;
    public float yPosMax;
    public float noise;

    public Vector4[] positions;
    public float[] radiuses;
    public float[] intensities;
    Vector4[] properties;

    public Material material;

    public int count = 50;

    void Start ()
    {
        positions = new Vector4[count];
        radiuses = new float[count];
        intensities= new float[count];
        properties = new Vector4[count];

        for (int i = 0; i < positions.Length; i++)
        {
            positions[i] = new Vector4(Random.Range(xPosMin, xPosMax), 0f, Random.Range(yPosMin, yPosMax),0f);
            radiuses[i] = Random.Range(0f, 100f);
            intensities[i] = Random.Range(-0.25f, 1f);
        }
    }

    void Update()
    {
        material.SetInt("_Points_Length", positions.Length);
        for (int i = 0; i < positions.Length; i++)
        {
            positions[i] += new Vector4(Random.Range(-noise, noise), Random.Range(-noise, noise),0) * Time.deltaTime ;
            properties[i] = new Vector4(radiuses[i], intensities[i], 0, 0);
        }

        material.SetVectorArray("_Points", positions);
        material.SetVectorArray("_Properties", properties);
    }
}