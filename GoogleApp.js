function doPost(e) {
  const doc = SpreadsheetApp.openById("YOUR_SPREADSHEET_ID");
  const sheet = doc.getSheetByName("Sheet1");

  const timestamp = new Date();
  const ipAddress = e.parameter.ipAddress;
  const name = e.parameter.name;
  const wishDate = e.parameter.wishDate;
  const wishItem = e.parameter.wishItem;
  const wishWhen = e.parameter.wishWhen;
  const remarks = e.parameter.remarks;

  sheet.appendRow([timestamp, ipAddress, name, wishDate, wishItem, wishWhen, remarks]);

  return ContentService.createTextOutput("Success");
}
