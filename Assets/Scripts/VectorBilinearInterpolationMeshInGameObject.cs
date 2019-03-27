﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VectorBilinearInterpolationMeshInGameObject : MonoBehaviour {

    public GameObject[] OriginBoxKeyPts;
    public GameObject[] TargetBoxKeyPts;
    public GameObject InputParentGO;
    public GameObject OutputParentGO;
    public List<GameObject> InputMeshGOList = new List<GameObject>();
    public List<GameObject> OutputMeshGOList = new List<GameObject>();

    private Vector3 a1, b1, c1, d1, e1, f1, g1, h1, a2, b2, c2, d2, e2, f2, g2, h2;
    private Vector3 t1, t2;
    private float u, v, w;
    
    void Start()
    {

        // duplicate the InputParentGO as OutputParentGO
        OutputParentGO = Instantiate(InputParentGO, InputParentGO.transform.parent);

        // disable the InputParentGO
        InputParentGO.SetActive(false);

    }
    
    void Update () {

        a1 = OriginBoxKeyPts[0].transform.position;
        b1 = OriginBoxKeyPts[1].transform.position;
        //c1 = OriginBoxKeyPts[2].transform.position;
        d1 = OriginBoxKeyPts[3].transform.position;
        e1 = OriginBoxKeyPts[4].transform.position;
        //f1 = OriginBoxKeyPts[5].transform.position;
        //g1 = OriginBoxKeyPts[6].transform.position;
        //h1 = OriginBoxKeyPts[7].transform.position;

        a2 = TargetBoxKeyPts[0].transform.position;
        b2 = TargetBoxKeyPts[1].transform.position;
        c2 = TargetBoxKeyPts[2].transform.position;
        d2 = TargetBoxKeyPts[3].transform.position;
        e2 = TargetBoxKeyPts[4].transform.position;
        f2 = TargetBoxKeyPts[5].transform.position;
        g2 = TargetBoxKeyPts[6].transform.position;
        h2 = TargetBoxKeyPts[7].transform.position;

        // find all meshes in InputGameObjectWithMesh and add to InputMeshList
        InputMeshGOList = new List<GameObject>();
        OutputMeshGOList = new List<GameObject>();
        Transform[] AllChildren = InputParentGO.GetComponentsInChildren<Transform>();
        foreach (Transform Child in AllChildren)
        {
            if (Child.gameObject.GetComponent<MeshFilter>() != null)
            {
                InputMeshGOList.Add(Child.gameObject);
            }
        }
        AllChildren = OutputParentGO.GetComponentsInChildren<Transform>();
        foreach (Transform Child in AllChildren)
        {
            if (Child.gameObject.GetComponent<MeshFilter>() != null)
            {
                OutputMeshGOList.Add(Child.gameObject);
            }
        }

        // loop all meshes in InputMeshList
        for (int i=0;i< InputMeshGOList.Count;i++)
        {
            Mesh meshInput = InputMeshGOList[i].GetComponent<MeshFilter>().mesh;
            Mesh meshOutput = OutputMeshGOList[i].GetComponent<MeshFilter>().mesh;

            Vector3[] verticesInput = meshInput.vertices;
            Vector3[] verticesOutput = meshOutput.vertices;
            int j = 0;
            while (j < verticesInput.Length) // loop each vertex
            {

                t1 = InputMeshGOList[i].transform.TransformPoint(verticesInput[j]);

                u = (t1[0] - a1[0]) / (b1[0] - a1[0]);
                v = (t1[2] - a1[2]) / (d1[2] - a1[2]);
                w = (t1[1] - a1[1]) / (e1[1] - a1[1]);

                t2 = BoxLerp(a2, b2, c2, d2, e2, f2, g2, h2, u, v, w);

                verticesOutput[j] = OutputMeshGOList[i].transform.InverseTransformPoint(t2);

                j++;
            }
            meshOutput.vertices = verticesOutput;
            
        }

    }
    
    // Thanks to Brian-Stone's answer
    // https://forum.unity.com/threads/vector-bilinear-interpolation-of-a-square-grid.205644/

    // Given a (u,v) coordinate that defines a 2D local position inside a planar quadrilateral, find the
    // absolute 3D (x,y,z) coordinate at that location.
    //
    //  0 <----u----> 1
    //  d ----------- c    1
    //  |             |   /|\
    //  |             |    |
    //  |             |    v
    //  |  *(u,v)     |    |
    //  |             |   \|/
    //  a ----------- b    0
    //
    // a, b, c, and d are the vertices of the quadrilateral. They are assumed to exist in the
    // same plane in 3D space, but this function will allow for some non-planar error.
    //
    // Variables u and v are the two-dimensional local coordinates inside the quadrilateral.
    // To find a point that is inside the quadrilateral, both u and v must be between 0 and 1 inclusive.  
    // For example, if you send this function u=0, v=0, then it will return coordinate "a".  
    // Similarly, coordinate u=1, v=1 will return vector "c". Any values between 0 and 1
    // will return a coordinate that is bi-linearly interpolated between the four vertices.


    //public Vector3 QuadLerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float u, float v)
    //{
    //    Vector3 abu = Vector3.Lerp(a, b, u);
    //    Vector3 dcu = Vector3.Lerp(d, c, u);
    //    Vector3 t = Vector3.Lerp(abu, dcu, v);
    //    return t;
    //}

    // improved 2D lerp, test point can be outside
    public Vector3 QuadLerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float u, float v)
    {
        Vector3 abu = (b-a)*u+a;
        Vector3 dcu = (c-d)*u+d;
        Vector3 t = (dcu-abu)*v+abu;
        return t;
    }

    // improved 3D lerp, test point can be outside
    public Vector3 BoxLerp(Vector3 a, Vector3 b, Vector3 c, Vector3 d, Vector3 e, Vector3 f, Vector3 g, Vector3 h, float u, float v, float w)
    {
        Vector3 abu = (b - a) * u + a;
        Vector3 dcu = (c - d) * u + d;
        Vector3 t1 = (dcu - abu) * v + abu;
        Vector3 efu = (f - e) * u + e;
        Vector3 hgu = (g - h) * u + h;
        Vector3 t2 = (hgu - efu) * v + efu;
        Vector3 t = (t2 - t1) * w + t1;
        return t;
    }

}
