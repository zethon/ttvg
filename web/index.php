<?php

define("PRODUCTION", 1);

if (PRODUCTION == 1)
{
    define("DOWNLOAD_PATH", "./downloads/");
    define('LOCAL_PATH_PREFIX', "/amb/public_html/");
}
else
{
    define("DOWNLOAD_PATH", "./");
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

function getMacDownloadFilename()
{
    $dir = glob(LOCAL_PATH_PREFIX . DOWNLOAD_PATH . 'tooter-*-darwin*.zip');
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
    <title>Adventures in Tucson</title>
    <!-- <link rel="icon" type="img/png" href="images/owl_32.png" /> -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <link href="css/bootstrap.min.css" rel="stylesheet">

    <style>
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
        margin: 5px;
        padding: 5px;
      }
    </style>
  
  </head>
  <body>

<center>
    <h1>Adventures in Tucson</h1>
    <br/>
    <a href="#about">About</a>&nbsp;&nbsp;&nbsp;
    <a href="https://github.com/zethon/ttvg/wiki" target="_blank">Wiki</a>&nbsp;&nbsp;&nbsp;
    <a href="https://github.com/zethon/ttvg" target="_blank">GitHub</a>&nbsp;&nbsp;&nbsp;
    <a href="#downloads">Download</a>&nbsp;&nbsp;&nbsp;
    <a href="https://www.youtube.com/channel/UC817umzuXFvE18cnsVx9JuA/videos" target="_blank">Videos</a>
    <br/>
    <br/>
    <img src="https://github.com/zethon/ttvg/raw/master/docs/images/screenshot1.png"/ width="55%">
</center>

<br/></br>

<a id="about"/><h2>About</h2>

<p>The Adventures in Tucson Video Game is an open source role-playing game. All characters and events in this video game, even those based on real people, are entirely fictional. This video game contains coarse language and due to its content it should not be played by anyone.</p>

<br/>

<a id="downloads"/><h2>Downloads</h2>

<h3>Windows</h3>

<!-- Download Path: <?php echo DOWNLOAD_PATH; ?> -->
<!-- Prefix Path  : <?php echo LOCAL_PATH_PREFIX; ?> -->

<p> 
<?php 
    $filename = getWindowsDownloadFilename();
    echo '<a href="http://amb.la/' . DOWNLOAD_PATH . $filename . '">' . $filename . '</a>';
    $filetime = date("d F Y H:i", filemtime(LOCAL_PATH_PREFIX . DOWNLOAD_PATH . $filename));
    echo '&nbsp;(' . $filetime . ' GMT)';
?>
</p>

<br/>

<h3>macOS</h3>
<p> 
<?php 
    $filename = getMacDownloadFilename();
    echo '<a href="http://amb.la/' . DOWNLOAD_PATH . $filename . '">' . $filename . '</a>';
    $filetime = date("d F Y H:i", filemtime(LOCAL_PATH_PREFIX . DOWNLOAD_PATH . $filename));
    echo '&nbsp;(' . $filetime . ' GMT)';
?>
</p>

<br/>
<h3>Ubuntu</h3>
<p>Coming soon...</p>
<br/>

</body>

<script src="https://code.jquery.com/jquery.js"></script>
<script src="js/bootstrap.min.js"></script>

</html>
