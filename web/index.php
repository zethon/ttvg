<?php

define("PRODUCTION", 1);

define("DOWNLOAD_PATH", "./downloads/");

if (PRODUCTION == 1)
{
    define('LOCAL_PATH_PREFIX', "/amb/public_html/");
}
else
{
    define('LOCAL_PATH_PREFIX', "");
}

function getWindowsDownloadFilename()
{
    $dir = glob(LOCAL_PATH_PREFIX . DOWNLOAD_PATH . 'tooter-*.exe');
    usort($dir, create_function('$a,$b', 'return filemtime($b) - filemtime($a);'));
    if (count($dir) > 0)
    {
        return basename($dir[0]);
    }

    return '';
}

?>
<!DOCTYPE html>
<html>
  <head>
    <title>The Tommy Tooter Video Game</title>
    <!-- <link rel="icon" type="img/png" href="images/owl_32.png" /> -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <link href="css/bootstrap.min.css" rel="stylesheet">

    <style>
      .ribbon-wrapper {
        width: 190px;
        height: 190px;
        overflow: hidden;
        position: absolute;
        top: -3px;
        left: -3px;
        margin-left: 15px;
      }

      .ribbon {
          color: #333;
          text-align: center;
          -webkit-transform: rotate(-45deg);
          -moz-transform:    rotate(-45deg);
          -ms-transform:     rotate(-45deg);
          -o-transform:      rotate(-45deg);
          position: relative;
          padding: 7px 0;
          top: 45px;
          left: -125px;
          width: 400px;
          background-color: #999;
          color: #fff;
      }
      .ribbon a {
        color: #fff;
      }

      @font-face {
        font-family: 'Open Sans';
        font-style: normal;
        font-weight: 300;
        src: local('Open Sans Light'), local('OpenSans-Light'), url(fonts/DXI1ORHCpsQm3Vp6mXoaTXhCUOGz7vYGh680lGh-uXM.woff) format('woff');
      }

      @font-face {
        font-family: 'Open Sans';
        font-style: normal;
        font-weight: 400;
        src: local('Open Sans'), local('OpenSans'), url(fonts/cJZKeOuBrn4kERxqtaUH3T8E0i7KZn-EPnyo3HZu7kw.woff) format('woff');
      }

      body, h1, h2, h3, h4, p 
      {
        font-family: "Helvetica Neue",Helvetica,Arial,sans-serif !important;
        font-weight: 300;
        margin: 0;
        padding: 0;
      }
    </style>
  
  </head>
  <body>

<h1>The Tommy Tooter Video Game</h1>

<br/><br/>

<h3>Windows</h3>

<!-- Download Path: <?php echo DOWNLOAD_PATH; ?> -->
<!-- Prefix Path  : <?php echo LOCAL_PATH_PREFIX; ?> -->

<br/>
<b>Link:</b>&nbsp; 
<?php 
    $filename = getWindowsDownloadFilename();
    echo '<a href="http://amb.la/' . DOWNLOAD_PATH . $filename . '">' . $filename . '</a>';
    $filetime = date("F d Y", filemtime(LOCAL_PATH_PREFIX . DOWNLOAD_PATH . $filename));
    echo '&nbsp;(' . $filetime . ')';
?> 

<br/><br/>

<h3>macOS</h3>
Coming soon...

<br/><br/>
<h3>Ubuntu</h3>
Coming soon...
<br/>

  </body>
    <script src="https://code.jquery.com/jquery.js"></script>
    <script src="js/bootstrap.min.js"></script>
</html>
