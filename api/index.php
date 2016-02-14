<?php

/**
 * スロットゲームのプレイログ受信・記録アプリ
 *
 * @copyright    2014 Obelisk, Y.K.
 * @author       obelisk@siren.ocn.ne.jp
 * @license
 *
 */
require 'rb.php';
require 'Slim/Slim.php';
require 'Log.php';

\Slim\Slim::registerAutoloader();

const DEBUG = true;


/*
 バリデーションがあれば・・・
class Model_Playlog extends RedBean_SimpleModel
{
    public function update() {
        if (...) {
            throw new Exception('');
        }
    }
};
 */


if (DEBUG) {
    $config = array(
        'debug' => DEBUG,
        'log.level' => \Slim\Log::DEBUG,
        'log.enabled' => true,
        'log.writer' => new \Log\Writer(),
        'cookies.encrypt' => true
    );
} else {
    $config = array();
}

$app = new \Slim\Slim($config);


$app->error(function(\Exception $e) use ($app) {
    echo $e;
    $code = $e->getCode();
    $status = 500;
    $message = 'サーバー内部でのエラー';
    if ($e instanceof \HttpException) {
        switch($code) {
            case 403:
                $message = 'ページは表示できない';
                $status = 403;
                break;
            case 404:
                $message = 'ページが見つからない';
                $status = 404;
                break;
            case 405:
                $message = '不正アクセス';
                $status = 405;
                break;
        }
    }
    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode(array(
        'status' => $status,
        'message' => $message
    )));
});

$app->hook('slim.before', function () use ($app) {
    $db_path = 'db/playlogs.db';
    /*
     * ファイルが存在しない場合、SQLiteがデータベースファイルを作成するので、
     * 要らないか・・・
    if (!file_exists($db_path)) {
        $app->response->headers->set('Content-Type', 'application/json');
        $app->response->setBody(json_encode(array(
            'status' => 500,
            'message' => 'the database file is not found.'
        )));
    }*/
    R::setup('sqlite:' . $db_path);
});

$app->group('/test', function() use ($app) {
    $app->post('/', 'test');
});

function test()
{
    $app = \Slim\Slim::getInstance();
    $body = $app->request->getBody();

    $json = json_decode($body);
    $method = (string)$json->method;
    $val1 = (int)$json->val1;
    $val2 = (int)$json->val2;

    $ret = new stdClass();
    $ret->method = $method;
    $ret->val1 = $val1;
    $ret->val2 = $val2;
    if ($method === 'add') {
        $ret->answer = $val1 + $val2;
    } elseif ($method === 'sub') {
        $ret->answer = $val1 - $val2;
    }
    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

$app->group('/playlogs', function() use ($app) {
    $app->group('/db', function() use ($app) {
        $app->get('/', function() use ($app) {
        });
        $app->post('/', function() use ($app) {
        });
        $app->put('/', function() use ($app) {
        });
        $app->delete('/', function() use ($app) {
        });
    });
    $app->get('/', 'getLogs');
    $app->get('/:id', 'getLog');
    $app->post('/', 'addLog');
    $app->put('/:id', 'updateLog');
    $app->delete('/:id', 'deleteLog');
});

$app->notFound(function() use ($app) {
    $ret = new stdClass();
    $ret->status = 404;
    $ret->message = 'ページが見つからない';

    $app->response->setStatus(404);
    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
});

function getLogs()
{
    $app = \Slim\Slim::getInstance();
    $ret = array();

    try {
        $logs = R::findAll('playlog');
        foreach ($logs as $log) {
            $obj = new stdClass();
            $obj->id = $log->id;
            $obj->uuid = $log->uuid;
            $obj->point = (int)$log->point;
            $obj->date = $log->date;
            $ret[] = $obj;
        }
    } catch (\RedBeanPHP\RedException\SQL $exp) {
        $ret = new stdClass();
        $ret->status = 'error';
        $ret->message = $exp->getMessage();
        $app->response->setStatus(500);
    }

    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

function getLog($id)
{
    $app = \Slim\Slim::getInstance();
    $ret = new stdClass();

    try {
        $log = R::load('playlog', $id);
        if ($log->id) {
            $ret->status = 'success';
            $ret->log = new stdClass();
            $ret->log->id = $log->id;
            $ret->log->uuid = $log->uuid;
            $ret->log->point = $log->point;
            $ret->log->date = $log->date;
        }
    } catch (\RedBeanPHP\RedException\SQL $exp) {
        $ret->status = false;
        $ret->message = $exp->getMessage();
        $app->response->setStatus(500);
    }

    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

function addLog()
{
    $app = \Slim\Slim::getInstance();
    // @TODO JSON以外も扱えるようにする
    // @TODO contentType()で処理を分ける
    //$type = $app->contentType();
    $body = $app->request->getBody();
    $json = json_decode($body);
    $ret = new stdClass();

    try {
        $log = R::dispense('playlog');
        $log->uuid = $json->uuid;
        $log->point = $json->point;
        $log->date = $json->date;
        $id = R::store($log);

        $ret->status = 'success';
        $ret->log = new stdClass();
        $ret->log->id = $id;
        $ret->log->uuid = $json->uuid;
        $ret->log->point = $json->point;
        $ret->log->date = $json->date;
        if (isset($json->filename)) {
            $ret->filename = $json->filename;
        }
    } catch (\RedBeanPHP\RedException\SQL $exp) {
        $ret->status = 'error';
        $ret->message = $exp->getMessage();
        $app->response->setStatus(500);
    }
    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

function updateLog($id)
{
    $app = \Slim\Slim::getInstance();
    $body = $app->request->getBody();
    $json = json_decode($body);
    $ret = new stdClass();

    try {
        $log = R::load('playlog', $id);

        $log->uuid = isset($json->uuid) ? $json->uuid : $log->uuid;
        $log->point = isset($json->point) ? $json->point : $log->point;
        $log->date = isset($json->date) ? $json->date : $log->date;

        $_id = R::store($log, $id);

        $ret->status = 'success';
        $ret->log = new stdClass();
        $ret->log->id = $_id;
        $ret->log->uuid = $log->uuid;
        $ret->log->point = $log->point;
        $ret->log->date = $log->date;
        if (isset($json->filename)) {
            $ret->filename = $json->filename;
        }

    } catch (\RedBeanPHP\RedException\SQL $exp) {
        $ret->status = 'error';
        $ret->message = $getMessage();
        $app->response->setStatus(500);
    }
    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

function deleteLog($id)
{
    $app = \Slim\Slim::getInstance();
    $ret = new stdClass();

    try {
        $log = R::load('playlog', $id);
        R::trash($log);

        $ret->status = true;
        $ret->message = 'successfully deleted!';
    } catch (\RedBeanPHP\RedException\SQL $exp) {
        $ret->status = false;
        $ret->message = $exp->getMessage();
    }

    $app->response->headers->set('Content-Type', 'application/json');
    $app->response->setBody(json_encode($ret));
}

$app->run();
