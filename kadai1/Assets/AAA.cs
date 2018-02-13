using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class AAA : MonoBehaviour
{
	// CSVファイル
	private List<string[]> csvDatas = new List<string[]> ();
	// CSVの中身を入れるリスト
	private int height = 0;
	// CSVの行数

	float time_sum = 0.0f;
	int i = 0;
	// Use this for initialization
	void Start ()
	{



		TextAsset csvFile = Resources.Load ("quaternion") as TextAsset;
		StringReader reader = new StringReader (csvFile.text);
		while (reader.Peek () > -1) {
			string line = reader.ReadLine ();
			csvDatas.Add (line.Split (',')); // リストに入れる
			height++; // 行数加算
		}
	}
	
	// Update is called once per frame
	void Update ()
	{
		time_sum += Time.deltaTime;
		Quaternion q = Quaternion.identity;
		q.w = float.Parse (csvDatas [i] [1]);
		q.x = float.Parse (csvDatas [i] [2]);
		q.y = float.Parse (csvDatas [i] [3]);
		q.z = float.Parse (csvDatas [i] [4]);
		Debug.Log (q.w);
		this.transform.rotation = q; // クォータニオンで回転させる
		i++;
	}
}
