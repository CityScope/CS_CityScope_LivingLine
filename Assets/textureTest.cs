using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using OpenCVForUnity.UnityUtils;

public class textureTest : MonoBehaviour
{

    public RenderTexture myRenderTexture;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {

        Texture2D inputTexture = new Texture2D(myRenderTexture.width, myRenderTexture.height, TextureFormat.ARGB32, false);

        Utils.textureToTexture2D(myRenderTexture, inputTexture);

        gameObject.GetComponent<Renderer>().material.mainTexture = inputTexture;
    }
}
