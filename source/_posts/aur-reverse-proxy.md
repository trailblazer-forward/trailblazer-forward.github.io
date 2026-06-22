---
title: 配置 AUR 反代的记录
tags: technology
date: 2026-06-23 09:53:00
---


## 前言

虽然 AUR 理论上仅提供 PKGBUILD 文件，然而访问 AUR 还是时不时抽风。

```
t_@tlex ~ Mon 22 20:45
 %> yay -Syu
:: 正在同步软件包数据库...
 core 已经是最新版本
 extra 已经是最新版本
 archlinuxcn 已经是最新版本
 multilib 已经是最新版本
:: 正在搜索 AUR 更新...

:: 正在搜索数据库更新...
 -> 1 error occurred:
	* request failed: Get "https://aur.archlinux.org/rpc?*****": EOF
```

为了提升访问的稳定性，考虑利用 Cloudflare Worker 搭建反向代理服务。

## Worker 代码

修改自 [清羽飞扬](https://blog.liushen.fun/posts/dd89adc9#%E4%B8%93%E7%94%A8%E8%BD%AC%E5%8F%91) 的博客

```javascript
const upstream = 'aur.archlinux.org'
const upstream_path = '/'
const upstream_mobile = 'aur.archlinux.org'
const blocked_region = ['KP', 'SY', 'PK', 'CU']
const blocked_ip_address = ['0.0.0.0', '127.0.0.1']
const https = true
const replace_dict = {'$upstream': '$custom_domain', '//aur.archlinux.org': ''}

addEventListener('fetch', event => {
  event.respondWith(fetchAndApply(event.request))
})
 
async function fetchAndApply(request) {
  const region = request.headers.get('cf-ipcountry').toUpperCase()
  const ip_address = request.headers.get('cf-connecting-ip')
  const user_agent = request.headers.get('user-agent')
 
  let response = null
  let url = new URL(request.url)
  let url_hostname = url.hostname
 
  if (https == true) {
    url.protocol = 'https:'
  } else {
    url.protocol = 'http:'
  }
 
  if (await device_status(user_agent)) {
    var upstream_domain = upstream
  } else {
    var upstream_domain = upstream_mobile
  }
 
  if (blocked_region.includes(region)) {
    response = new Response('Access denied: WorkersProxy is not available in your region yet.', {
      status: 403
    })
  } else if (blocked_ip_address.includes(ip_address)) {
    response = new Response('Access denied: Your IP address is blocked by WorkersProxy.', {
      status: 403
    })
  } else if (url.pathname == '/') {
    response = new Response('Access denied: Unsupport access to html page directly.\n\
Tips: use \'yay --aururl "https://aur.lolic0n.top" ...\' to use this site.', {
      status: 403
    })
  } else {
    let method = request.method
    let request_headers = request.headers
    let new_request_headers = new Headers(request_headers)
    
    url.host = upstream_domain
    url.pathname = upstream_path + url.pathname
    new_request_headers.set('Host', url.hostname)
    new_request_headers.set('Referer', url.hostname)

    let fetchOptions = {
      method: method,
      headers: new_request_headers
    }

    if (method === 'POST' || method === 'PUT' || method === 'PATCH') {
      fetchOptions.body = request.body;
    }

    let original_response = await fetch(url.href, fetchOptions)

    let original_text = null
    let response_headers = original_response.headers
    let new_response_headers = new Headers(response_headers)
    let status = original_response.status

    new_response_headers.set('access-control-allow-origin', '*')
    new_response_headers.set('access-control-allow-credentials', true)
    new_response_headers.delete('content-security-policy')
    new_response_headers.delete('content-security-policy-report-only')
    new_response_headers.delete('clear-site-data')
    
    const content_type = new_response_headers.get('content-type')
    if (content_type.includes('text/html') && content_type.includes('UTF-8')) {
      let original_response_clone = original_response.clone()
      original_text = await replace_response_text(original_response_clone, upstream_domain, url_hostname)
    } else {
      original_text = await original_response.arrayBuffer()
    }

    response = new Response(original_text, {
      status,
      headers: new_response_headers
    })
  }
  return response
}
 
async function replace_response_text(response, upstream_domain, host_name) {
  let text = await response.text()
 
  var i, j
  for (i in replace_dict) {
    j = replace_dict[i]
 
    if (i == '$upstream') {
      i = upstream_domain
    } else if (i == '$custom_domain') {
      i = host_name
    }
 
    if (j == '$upstream') {
      j = upstream_domain
    } else if (j == '$custom_domain') {
      j = host_name
    }
 
    let re = new RegExp(i, 'g')
    text = text.replace(re, j)
  }
  return text
}
 
async function device_status(user_agent_info) {
  var agents = ["Android", "iPhone", "SymbianOS", "Windows Phone", "iPad", "iPod"]
  var flag = true
  for (var v = 0; v < agents.length; v++) {
    if (user_agent_info.indexOf(agents[v]) > 0) {
      flag = false
      break
    }
  }
  return flag
}
```

最终成品部署于 aur.lolic0n.top 上了，想要用的可以直接用。

## 杂记

一开始尝试通过代理访问 aur.archlinux.org 首页，结果被 Anubis 拦了，折腾半天尝试绕过去。最后才发现仅代理 AUR 不需要绕 Anubis......

## 参考

- [使用 Cf Workers 搭建反代加速器](https://blog.liushen.fun/posts/dd89adc9)
