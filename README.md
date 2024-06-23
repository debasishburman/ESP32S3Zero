1. Google Apps Script to Save Wish Data
First, create a Google Apps Script to handle the incoming wish data and store it in a Google Sheet.

Open Google Sheets and create a new sheet.
Click on Extensions > Apps Script to open the Apps Script editor.
Delete any existing code and replace it with the following script:


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

Replace "YOUR_SPREADSHEET_ID" with your actual Google Sheet ID (found in the URL of your Google Sheet).

Save the script and deploy it as a web app by clicking on Deploy > New deployment.
