<?php 


function create_sprite ($files){
	global  $svgmap;
	$svg_string = '<svg xmlns="http://www.w3.org/2000/svg">' . PHP_EOL;
	$id_array = array();

	foreach ($files as $file) {
		if ( is_numeric($file['name']) ) {
			$url = wp_get_attachment_image_src($file['name']);

			if ( $url ) {
				$path = str_replace($wp_upload_dir['baseurl'], $wp_upload_dir['basedir'], $url[0]);

				$svg = simplexml_load_file($path);

				if ( $svg ) {
					array_push($id_array, $id);

					$viewbox = $svg->attributes()->viewBox;

					$svg_string .= '<symbol id=":'.$file['value'].'" viewBox="' . $viewbox . '">';

					foreach ($svg->children() as $child) {
						$svg_string .= preg_replace('/(\t|\n)+/',' ',$child->asXML());
					}

					$svg_string .= '</symbol>' . PHP_EOL;
				}
			}
		}
	}

	$svg_string .= '</svg>';

	// $res = file_put_contents($svgmap->svg_path, $svg_string);
	// file_put_contents($svgmap->ids_path, json_encode($id_array));

	return $svg_string;
}

// add_action('admin_print_footer_scripts', 'my_action_javascript', 99);
function my_action_javascript() {
	?>
	
	<?php
}


function my_action_callback() {
	global $svgmap;
	$array_data = $_POST;
	$resive = array ();
	
	$ret = create_sprite ($array_data['selected']);
	if ($ret) {
		$url_file = $svgmap->path.'/'.$array_data['file_name'].'.svg';
		$save_ok = file_put_contents($url_file, $ret);
		$id = 0;

		if ( false != $save_ok ) {
			if ($array_data['add_upload'] === 'on' ){
				$url = $svgmap->svg_url.'/'.$array_data['file_name'].'.svg';
				$file_array = [
					'name'     => basename( $url ),
					'tmp_name' => download_url( $url )
				];
				$id = media_handle_sideload( $file_array, 0 );

				if( is_wp_error( $id ) ){
					$resive['error'] = $id;
				}
				@unlink( $ret );
			} 
		} else {
			$resive['error'] = 'Hевозможно сохранить файл.Обратитесь к Администратору';
		}
	}
	if (!array_key_exists ( 'error', $resive )) {
		$resive['resive'] = 'ok';
		$resive['data'] = array(
			'file_size' => filesize ( $svgmap->path.'/'.$array_data['file_name'].'.svg' ) ,
			'file_path' => $svgmap->svg_url.'/'.$array_data['file_name'].'.svg',
			'post_id'   => $id,
			'file_name' => $array_data['file_name'],
			'post_link' => get_edit_post_link($id)

		);
	}
	echo (json_encode($resive));
	
	wp_die(); // выход нужен для того, чтобы в ответе не было ничего лишнего, только то что возвращает функция
}