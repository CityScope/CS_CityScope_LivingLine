using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections.Generic;
using OpenCVForUnity.CoreModule;
using OpenCVForUnity.ImgprocModule;
using OpenCVForUnity.UnityUtils;

public class WrapPerspective : MonoBehaviour
{

    public RenderTexture myRenderTexture;
    public Texture2D inputTexture;
    public GameObject[] corners;

    // Use this for initialization
    void Start()
    {
        inputTexture = new Texture2D(myRenderTexture.width, myRenderTexture.height, TextureFormat.ARGB32, false);
    }

    // Update is called once per frame
    void Update()
    {
        //Texture2D inputTexture = Resources.Load("lena") as Texture2D;
        
        Utils.textureToTexture2D(myRenderTexture, inputTexture);
        
        Mat inputMat = new Mat(inputTexture.height, inputTexture.width, CvType.CV_8UC4);

        Utils.texture2DToMat(inputTexture, inputMat);
        //Debug.Log("inputMat.ToString() " + inputMat.ToString());


        Mat src_mat = new Mat(4, 1, CvType.CV_32FC2);
        Mat dst_mat = new Mat(4, 1, CvType.CV_32FC2);


        src_mat.put(0, 0, // put() starting row, starting col
            0.0, 0.0, // top-left
            inputMat.rows(), 0.0, // top-right
            0.0, inputMat.cols(), // bottom-left
            inputMat.rows(), inputMat.cols() // bottom-right
            );
        dst_mat.put(0, 0, // put() starting row, starting col
            100.0, 0.0, // top-left
            inputMat.rows(), 100.0, // top-right
            0.0, inputMat.cols(), // bottom-left
            inputMat.rows(), inputMat.cols() - 200.0 // bottom-right
            );
        // put(int row, int col, float[] data)
        // put(int row, int col, double...data)
        // getPerspectiveTransform takes: Mat src and Mat dst. Those are MarOfPoints2f with src(top-left, top-right, bottom-left, bottom-right) 
        // of your contour and corresponding mat dst( 0,0 ; 0,449 ; 449,0 ; 449,449)
        
        Mat perspectiveTransform = Imgproc.getPerspectiveTransform(src_mat, dst_mat);


        Mat outputMat = inputMat.clone();

        
        Imgproc.warpPerspective(inputMat, outputMat, perspectiveTransform, new Size(inputMat.rows(), inputMat.cols()));


        Texture2D outputTexture = new Texture2D(outputMat.cols(), outputMat.rows(), TextureFormat.ARGB32, false);


        Utils.matToTexture2D(outputMat, outputTexture);
        
        gameObject.GetComponent<Renderer>().material.mainTexture = outputTexture;
    }
}