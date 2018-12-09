using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Console = System.Diagnostics.Debug;

namespace PhotoGrouper
{
	public class ImageInfo
	{
		public DateTime? TakenDate;
		public string TakenBy;
		public string LocationUrl;
		private string _locationUrl = @"https://www.google.com/maps/search/?api=1&query={0},{1}";
		private Dictionary<String, String> _imageTypeDictionary;
		private PropertyItem[] propItems;
		private System.Text.ASCIIEncoding encoding;
		private float? lat, lon;
		private Regex regex = new Regex(Regex.Escape(":"));
        public string Destination;
        public string FileName;

		public ImageInfo(string path)
		{
            string[] arr = path.Split('\\');
            FileName = arr[arr.Length - 1];
            _imageTypeDictionary = new Dictionary<String, String>();
			encoding = new System.Text.ASCIIEncoding();
            try
            {
                Image image = new Bitmap(path);
    			propItems = image.PropertyItems;
                TakenDate = GetTakenDate(image, new int[] { 36868, 306, 36867 });
                TakenBy = GetTakenBy(image, new int[] { 272, 271, 20518 });
 		    	lat = GetLATLON(image, 1, 2);
			    lon = GetLATLON(image, 3, 4);
			    LocationUrl = String.Format(_locationUrl, lat, lon);
                image.Dispose();
                if (TakenDate == null)
	    		{
                    FileInfo ifo = new FileInfo(path);
			    	TakenDate = ifo.CreationTime;
			    }
            }
            catch { };
            Destination = TakenDate?.ToString("yyyy") + "\\" + TakenDate?.ToString("yyyy-MM");
		}

        public float? GetLATLON(Image targetImg, int refInt, int valueInt)
		{
			try
			{
				PropertyItem propItemRef = targetImg.GetPropertyItem(refInt);
				PropertyItem propItemLong = targetImg.GetPropertyItem(valueInt);
				return ExifGpsToFloat(propItemRef, propItemLong);
			}
			catch (ArgumentException)
			{
				return null;
			}
		}
	
		private float ExifGpsToFloat(PropertyItem propItemRef, PropertyItem propItem)
		{
			uint degreesNumerator = BitConverter.ToUInt32(propItem.Value, 0);
			uint degreesDenominator = BitConverter.ToUInt32(propItem.Value, 4);
			float degrees = degreesNumerator / (float)degreesDenominator;

			uint minutesNumerator = BitConverter.ToUInt32(propItem.Value, 8);
			uint minutesDenominator = BitConverter.ToUInt32(propItem.Value, 12);
			float minutes = minutesNumerator / (float)minutesDenominator;

			uint secondsNumerator = BitConverter.ToUInt32(propItem.Value, 16);
			uint secondsDenominator = BitConverter.ToUInt32(propItem.Value, 20);
			float seconds = secondsNumerator / (float)secondsDenominator;

			float coorditate = degrees + (minutes / 60f) + (seconds / 3600f);
			string gpsRef = System.Text.Encoding.ASCII.GetString(new byte[1] { propItemRef.Value[0] }); //N, S, E, or W
			if (gpsRef == "S" || gpsRef == "W")
				coorditate = 0 - coorditate;
			return coorditate;
		}

		public DateTime? GetTakenDate(Image image, int[] arr)
		{
			foreach (var i in arr)
			{
				if (image.PropertyIdList.Any(p => p == i))
				{
                    try
                    {
                        var item = image.GetPropertyItem(i);
                        var hstr = regex.Replace(encoding.GetString(item.Value), "-", 2);
                        return DateTime.Parse(hstr);
                    }
                    catch
                    {
                        continue;
                    }
                }
			}
			return null;
		}


		public string GetTakenBy(Image image, int[] arr)
		{
			foreach (var i in arr)
			{
				if (image.PropertyIdList.Any(p => p == i))
				{
                    try
                    {
                        var item = image.GetPropertyItem(i);
                        return encoding.GetString(item.Value);
                    }
                    catch
                    {
                        continue;
                    }
				}
			}
			return "";
		}
 	}
}


