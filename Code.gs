function ReadableDate(timestamp) {
  var d = new Date(timestamp);
  return d.getFullYear() + "-" + ("00" + (d.getMonth() + 1)).slice(-2) + "-" + ("00" + d.getDate()).slice(-2) + " " +
    ("00" + d.getHours()).slice(-2) + ":" + ("00" + d.getMinutes()).slice(-2) + ":" + ("00" + d.getSeconds()).slice(-2);
}

function ColorCode(SYS, DIA) {
  if (DIA < 80) {
    if (SYS < 120) return "lime";
    else if (SYS < 130) return "yellow";
  } else if (SYS > 180 || DIA > 120) return "red";
  else if (SYS >= 140 || DIA >= 90) return "brown";
  else if (SYS >= 130 || DIA >= 80) return "orange";
  else return "white";
}

// Run once HTTP post comes from Helium
function doPost(e) {
  var data = JSON.parse(e.postData.contents);
  var GS = SpreadsheetApp.openById('<SheetID>');
  //var SheetDate = new Date().toLocaleDateString();
  ThisSheet = GS.getSheetByName("Current Values");
  // Get all names and device UIDs
  var KeyValue = GS.getSheetByName("Patient-Device").getDataRange().getValues();
  // Device data with placeholder in case we don't have a name associated with the device
  var ThisRecord = [data.Device, data.SYS, data.DIA, data.PPM, data.Temp, ReadableDate(Date.now())];
  // Lookup patient name
  KeyValue.forEach(function (i) { if (i[1] == data.Device) { ThisRecord.shift(); ThisRecord.unshift(i[0]); } });
  // Save to spreadsheet
  ThisSheet.getRange(ThisSheet.getLastRow() + 1, 1, 1, ThisRecord.length).setValues([ThisRecord]);
  // Add color
  ThisSheet.getRange(ThisSheet.getLastRow(), 7).setBackground(ColorCode(ThisRecord[1], ThisRecord[2]));
}
