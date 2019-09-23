<div class="wrap">
	<h2><?php print $svgmap->name; ?></h2>

	<ul class="subsubsub">
		<li><span class="svg-count-label">All</span> <span class="svg-count all">(<?php print($wp_query_posts_count); ?>)</span> |</li>
		<li><span class="svg-count-label">In Use</span> <span class="svg-count inuse">(<?php print($id_array_length); ?>)</span></li>
	</ul>

	<br class="clear">

	<form class="svg-list" method="post" id="svgmap_form_data">
		<?php

		$names = array();

		foreach ($wp_query_posts as $post) {
			$checked = in_array($post->ID, $id_array) ? ' checked' : '';
			$name = sanitize_title($post->post_title);

			if ( isset($names[$name]) ):
				$names[$name]++;
				$name = $name . '-' . $names[$name];
			else:
				$names[$name] = 1;
			endif;

			?>
			<label class="svg-listitem">
				<input class="svg-checkbox" type="checkbox" name="<?php print $post->ID; ?>" value="<?php print $name; ?>"<?php print $checked; ?> data-size="<?php print $post->size; ?>">
				<span class="svg-details">
					<img class="svg-image" src="<?php print $post->guid; ?>">
					<span class="svg-name"><?php print $name; ?> <a href="<?php echo get_edit_post_link($post->ID); ?>" target="_blank" class="button button-small">Edit</a></span>
				</span>
			</label>
		<?php } ?>
	</form>
	<form id="svgmap_form">
		<div class="svg-panel">
			<p class="svg-panelitem" style="float: left;">
				Spritemap URL: <strong><?php echo (' '.$svgmap->svg_url).'/' ?></strong><input id='file_name' class="svg-url" name="file_name" value="<?php echo $svgmap->def_file_name ; ?>" type="text" >
				<p  style="float: left;"><strong>.svg</strong></p>

				<p style="color: blue"><input type="checkbox"  name="add_upload" style="color: blue; margin-left: 10px"> добавить в uploads</p>
			</p>

			

			<p class="svg-panelitem">
				<span>Estimated filesize: <strong class="svg-filesize">Unknown</strong></span>

				// <?php //wp_nonce_field('svgmap_submit'); ?>
				
				<button class="svg-submit button button-primary" id="svg-submit" >Update <?php print $svgmap->name; ?></button>

			</p>
		</div>
		<input type="hidden" name="action" value="my_action">
	</form>
	<div id="svg-parent_popup_click" class="svg-parent_popup_click">
		<div class="svg-popup_click">
			<h3>При выполнения запроса обнаруженны <strong style='color:red;'>Ошибки</strong></h3>
			<p><center id="svg_err_text"></center>
				<a class="svg-close" title="Закрыть" onclick="document.getElementById('svg-parent_popup_click').style.display='none';">X</a>
			</div>
		</div> 
	</div>

	<form action="options.php" method="post">
		<?php
		settings_fields( $svgmap->id );
		do_settings_sections( $svgmap->id );
		submit_button();
		?>
	</form>

	<style><?php include $svgmap->path.'/style.css'; ?></style>
	<script><?php include $svgmap->path.'/script.js'; ?></script>
	<script>
		jQuery(document).ready(function($) {
			$('#svg-submit').click(function(e){
				e.preventDefault();
				var dataParams = {};
				var svgmap_form = $("#svgmap_form").serializeArray();
				for (var i = 0; i < svgmap_form.length; i++) {
					dataParams[svgmap_form[i]['name']] = svgmap_form[i]['value'];
				}
				dataParams.selected = $("#svgmap_form_data").serializeArray()
				var svg_submit_text =  $('#svg-submit').text();
				$.ajax({
					url: ajaxurl,
					type: 'POST',
					data: dataParams,
					beforeSend : function (){
						$('#preloader_svg').css("display","block");
						$('#svg-submit').text('Загрузка, 5 сек...');

					},
					success: function( data ) {
						var s = jQuery.parseJSON(data);
						var text = '';
						if (s.error) {
							if (typeof(s.error) === 'object') {
								text = JSON.stringify(s.error)+'<br> Обратитесь к администратору';
							} else {
								text = s.error;
							}
							$('#svg_err_text').text(text);
							$('#svg-parent_popup_click').css("display","block");
						};
						if (s.resive === 'ok') { 
							if (dataParams['add_upload'] === 'on') {
								text = '<label class="svg-listitem">';
								text +='<input class="svg-checkbox" type="checkbox" name="'+s.data.post_id+'" value="'+s.data.file_name+'" data-size="'+s.data.file_size+'">';
								text += '<span class="svg-details">';
								text += '<svg class="svg-image" src="'+s.data.file_path+'">';
								text += '<span class="svg-name">'+s.data.file_name+'<a href="'+s.data.post_link+'" target="_blank" class="button button-small">Edit</a></span>';

								text += '</label>';
								$('#svgmap_form_data').append(text);
							};
						};
						$('#svg-submit').text(svg_submit_text);	
						$('#preloader_svg').css("display","none");
					}
				});
			});
			var file_name = $('#file_name');
			$('#file_name').css({ 
				'width' :  ((file_name.val().length+2) * 8) + 'px'
			});
			var svg_def_file_name = file_name.val();

			$('#file_name').on('keyup', function(e) {
				console.log(e.key);

				if ($(this).val() === '') {
					$(this).val(svg_def_file_name);
				} else {
					var outString = $(this).val().replace(/[`~!@#$%^&*()_|+\-=?;:'",.<>\{\}\[\]\\\/]/gi, '');
					$(this).val(outString) ;
				}
				this.style.width = ((this.value.length) * 8)+8 + 'px';
			});

			var preloader_svg = "<div id='preloader_svg' class='preloader_svg'><div><img src='<?php echo ($svgmap->svg_url."/preloader.svg")?>'></div></div>";
			$( preloader_svg ).insertAfter( "#svg-parent_popup_click" );

		});
	</script>

