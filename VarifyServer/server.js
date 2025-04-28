const grpc = require("@grpc/grpc-js")
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');
const redis_module = require('./redis')


// Redis读超时函数
function redisGetWithTimeout(key, timeoutMs) {
    return Promise.race([
        redis_module.GetRedis(key),
        new Promise((_, reject) => setTimeout(() => reject(new Error("Redis timeout")), timeoutMs))
    ]);
}

// 核心 GetVarifyCode 实现
async function GetVarifyCode(call, callback) {
    const email = call.request.email;
    if (!email) {
        callback(null, {
            email: "",
            error: const_module.Errors.InvalidParams
        });
        return;
    }

    try {
        let query_res = null;
        try {
            query_res = await redisGetWithTimeout(const_module.code_prefix + email, 50); // 最多等50ms
        } catch (e) {
            console.warn(`Redis get timeout for email: ${email}, treat as no code`);
        }

        if (query_res != null) {
            // 命中缓存，直接返回
            if (Math.random() < 0.05) {  // 5%的采样日志
                console.log(`[Cache Hit] email=${email}, code=${query_res}`);
            }
            callback(null, {
                email: email,
                error: const_module.Errors.Success
            });
            return;
        }

        // 没有命中缓存，生成新的验证码
        let uniqueId = uuidv4().slice(0, 4);

        // 异步写Redis，不阻塞
        redis_module.SetRedisExpire(const_module.code_prefix + email, uniqueId, 180)
            .catch((err) => {
                console.error(`[Redis Set Failed] email=${email}, err=${err}`);
            });

        // 这里可以选择发送邮件（目前跳过）
        /*
        let text_str = '您的验证码为' + uniqueId + '，请三分钟内完成注册';
        let mailOptions = {
            from: 'shu920964205@163.com',
            to: email,
            subject: '验证码',
            text: text_str,
        };
        emailModule.SendMail(mailOptions).catch(err => console.error(`[Email Send Failed] email=${email}, err=${err}`));
        */

        // 快速返回给客户端
        if (Math.random() < 0.05) {
            console.log(`[New Code Generated] email=${email}, code=${uniqueId}`);
        }
        callback(null, {
            email: email,
            error: const_module.Errors.Success
        });

    } catch (error) {
        console.error(`[Server Exception] email=${call.request.email}, error=`, error);
        callback(null, {
            email: email,
            error: const_module.Errors.Exception
        });
    }
}

// 启动 gRPC Server
function main() {
    const server = new grpc.Server();
    server.addService(message_proto.VarifyService.service, {
        GetVarifyCode: GetVarifyCode
    });
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start();
        console.log('gRPC server started at 0.0.0.0:50051');
    });
}

main();