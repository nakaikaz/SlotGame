<?php

namespace Log {

    class Writer
    {
        public function write($message, $level = \Slim\Log::DEBUG)
        {
            echo $level . ': ' . $message . '<br />';
        }
    }

}
